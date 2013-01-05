/*! \file	containerinfo.cpp
 *	\brief	
 *
 *	\version $Id$
 *
 */
/* 
 *  This software is provided 'as-is', without any express or implied warranty.
 *  In no event will the authors be held liable for any damages arising from
 *  the use of this software.
 *  
 *  Permission is granted to anyone to use this software for any purpose,
 *  including commercial applications, and to alter it and redistribute it
 *  freely, subject to the following restrictions:
 *  
 *   1. The origin of this software must not be misrepresented; you must
 *      not claim that you wrote the original software. If you use this
 *      software in a product, you must include an acknowledgment of the
 *      authorship in the product documentation.
 *  
 *   2. Altered source versions must be plainly marked as such, and must
 *      not be misrepresented as being the original software.
 *  
 *   3. This notice may not be removed or altered from any source
 *      distribution.
 */

#include "containerinfo.h"

#include "mxflib/mxflib.h"
using namespace mxflib;

//! Build an ContainerInfo for the essence in a given file
/*! \return NULL on error
 */
ContainerInfo* ContainerInfo::CreateAndBuild(MXFFilePtr &File)
{
	// Build en empty set of info to return
	ContainerInfo* Ret = new ContainerInfo;

	// Get the master metadata set (or the header if we must)
	PartitionPtr MasterPartition = File->ReadMasterPartition();
	if(!MasterPartition)
	{
		File->Seek(0);
		MasterPartition = File->ReadPartition();
		warning("File %s does not contain a closed copy of header metadata - using the open copy in the file header\n", File->Name.c_str());
	}

	if(!MasterPartition) 
	{
		error("Could not read header metadata from file %s\n", File->Name.c_str());
		return NULL;
	}

	// Read and parse the metadata
	MasterPartition->ReadMetadata();
	Ret->HMeta = MasterPartition->ParseMetadata();
	
	if(!Ret->HMeta) 
	{
		error("Could not read header metadata from file %s\n", File->Name.c_str());
		return false;
	}

	/* Scan the Essence container data sets to get PackageID to BodySID mapping */
	MDObjectPtr ECDSet = Ret->HMeta[ContentStorageObject_UL];
	if(ECDSet) ECDSet = ECDSet->GetLink();
	if(ECDSet) ECDSet = ECDSet[EssenceDataObjects_UL];
	if(!ECDSet)
	{
		error("Header metadata in file %s does not contain an EssenceContainerData set\n", File->Name.c_str());
		return false;
	}

	MDObject::iterator it = ECDSet->begin();
	while(it != ECDSet->end())
	{
		MDObjectPtr ThisECDSet = (*it).second->GetLink();
		MDObjectPtr PackageID;
		if(ThisECDSet) PackageID = ThisECDSet->Child(LinkedPackageUID_UL);
		if(PackageID)
		{
			EssenceTrackDescriptorPtr NewEI = new EssenceTrackDescriptor;
			NewEI->PackageID = new UMID(PackageID->PutData()->Data);

			// Insert the basic essence info - but not if this is external essence (BodySID == 0)
			UInt32 BodySID = ThisECDSet->GetUInt(BodySID_UL);
			if(BodySID) Ret->Lookup[BodySID] = NewEI;
		}
		it++;
	}

	/* Now find the other items for the essence lookup map */
	if(Ret->Lookup.size())
	{
		PackageList::iterator it = Ret->HMeta->Packages.begin();
		while(it != Ret->HMeta->Packages.end())
		{
			// Only Source Packages are of interest
			if((*it)->IsA(SourcePackage_UL))
			{
				MDObjectPtr Descriptor = (*it)->Child(Descriptor_UL);
				if(Descriptor) Descriptor = Descriptor->GetLink();

				if(Descriptor)
				{
					MDObjectPtr PackageID = (*it)->Child(PackageUID_UL);
					if(PackageID)
					{
						UMIDPtr TheID = new UMID(PackageID->PutData()->Data);
						
						/* Now do a lookup in the essence lookup map (it will need to be done the long way here */
						EssenceTrackDescriptorMap::iterator EL_it = Ret->Lookup.begin();
						while(EL_it != Ret->Lookup.end())
						{
							if((*((*EL_it).second->PackageID)) == (*TheID))
							{
								// If found, set the missing items and stop searching
								(*EL_it).second->Package = (*it);
								(*EL_it).second->Descriptor = Descriptor;
								
								if(Descriptor->IsA(MultipleDescriptor_UL))
								{
									// if the descriptor is a multiple descriptor we will need to fill the map of track number to descriptor
									MDObjectPtr DescriptorList = Descriptor[FileDescriptors_UL];
									(*EL_it).second->FillDescriptorMap(*it, DescriptorList);
								}
								else
								{
									// otherwise, just add this one
									(*EL_it).second->AddSoleDescriptor(*it, Descriptor);
								}

								break;
							}
							EL_it++;
						}
					}
				}
			}

			it++;
		}
	}

	return Ret;
}

//! Add the sole Descriptor to the map
void EssenceTrackDescriptor::AddSoleDescriptor(PackagePtr &Package, MDObjectPtr Descriptor)
{
	TrackList::iterator Track_it = Package->Tracks.begin();
	while(Track_it != Package->Tracks.end())
	{
		// use first Essence Track
		if((*Track_it)->IsEssenceTrack())
		{
			UInt32 TrackNumber = (*Track_it)->GetUInt(TrackNumber_UL);
			TrackNumDescriptors[TrackNumber] = Descriptor;
		
			// in order of preference, choose TrackID, LinkedTrackID, or dummy (-1)
			UInt32 TrackID = -1; // dummy
			MDObjectPtr TrackIDObject = (*Track_it)->Child(TrackID_UL);
			if(TrackIDObject) TrackID = TrackIDObject->GetUInt();

			UInt32 LinkedTrackID = -1; // dummy
			MDObjectPtr LinkedTrackIDObject = Descriptor->Child(LinkedTrackID_UL);
			if(LinkedTrackIDObject) LinkedTrackID = LinkedTrackIDObject->GetUInt();

			if( TrackID == -1 )
			{
				// use LinkedTrackID if there is one
				TrackIDDescriptors[LinkedTrackID] = Descriptor;
			}
			else if( LinkedTrackID == -1 || LinkedTrackID == TrackID )
			{
				// there is a TrackID and LinkedTrackID does not exist, or matches
				TrackIDDescriptors[TrackID] = Descriptor;
			}
			else
			{
				// LinkedTrackID does not match TrackID - better use a dummy
				TrackIDDescriptors[-1] = Descriptor;
			}

			break;
		}
		else
		{
			Track_it++;
		}
	}
}

//! Build a map of track number to descriptor where the descriptor is a multiple descriptor
void EssenceTrackDescriptor::FillDescriptorMap(PackagePtr &Package, MDObjectPtr DescriptorList)
{
	if(!DescriptorList) return;

	int DummyTrackID = -1; // msb == 1 not possible in real world TrackIDs

	TrackList::iterator Track_it = Package->Tracks.begin();
	while(Track_it != Package->Tracks.end())
	{
		// skip non Essence Tracks
		if(!(*Track_it)->IsEssenceTrack())
		{
			Track_it++;
			continue;
		}

		UInt32 TrackID;
		bool HasTrackID = false;

		MDObjectPtr TrackIDObject = (*Track_it)->Child(TrackID_UL);
		if(TrackIDObject)
		{
			HasTrackID = true;
			TrackID = TrackIDObject->GetUInt();
		}

		UInt32 TrackNumber = (*Track_it)->GetUInt(TrackNumber_UL);
	
		// in order of preference, choose TrackID, LinkedTrackID, or position-based, or dummy (-1)
		int TrackPos = 0;
		int DescriptorPos = 0;
		MDObject::iterator it = DescriptorList->begin();
		while(it != DescriptorList->end())
		{
			MDObjectPtr SubDescriptor = (*it).second->GetLink();
			if(SubDescriptor)
			{
				if(!HasTrackID)
				{
					// Track has no TrackID parameter, fall-back to position linking
					if(DescriptorPos == TrackPos)
					{
						TrackNumDescriptors[TrackNumber] = SubDescriptor;
						TrackIDDescriptors[DummyTrackID--] = SubDescriptor;
						break;
					}
				}
				else
				{
					MDObjectPtr LinkedTrackIDObject = SubDescriptor->Child(LinkedTrackID_UL);
					if(LinkedTrackIDObject)
					{
						UInt32 LinkedTrackID = LinkedTrackIDObject->GetUInt();

						if(LinkedTrackID == TrackID)
						{
							TrackNumDescriptors[TrackNumber] = SubDescriptor;
							TrackIDDescriptors[TrackID] = SubDescriptor;
							break;
						}
					}
					else
					{
						// Descriptor has no LinkedTrackID parameter, fall-back to position linking
						if(DescriptorPos == TrackPos)
						{
							TrackNumDescriptors[TrackNumber] = SubDescriptor;
							TrackIDDescriptors[DummyTrackID--] = SubDescriptor;
							break;
						}
					}
				}
			}

			DescriptorPos++;
			it++;
		}

		// TODO if position-based failed, it means there are fewer Descriptors than Essence Tracks
	
		TrackPos++;
		Track_it++;
	}
}
