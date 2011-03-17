

#include "mxflib.h"

using namespace mxflib;


//! Add a new metadata instance, without adding any know sources
MetadataInfo* MetaBuilder::AddInstance(void)
{
	// Add a new empty instance
	MetadataInfo *NewInstance = new MetadataInfo(this);
	Instances.push_back(NewInstance);

	return NewInstance;
}


//! Add a new metadata instance, with a specified internal source
MetadataInfo* MetaBuilder::AddInstance(UInt32 BodySID, EssenceSourcePtr &Source, WrappingConfigPtr &Config)
{
	// Add a new instance for the source
	MetadataInfo *NewInstance = new MetadataInfo(this, BodySID, Source, Config);
	Instances.push_back(NewInstance);

	return NewInstance;
}


//! Construct a basic MetadataInfo
MetadataInfo::MetadataInfo(MetaBuilder *Parent) : Parent(Parent), pFilePackage(NULL)
{
	// Add a material package and set it as the master
	Master = new PackageInfo(this, true);
	Packages.push_back(Master);

	pMaterialPackage = Master;
}


//! Construct a new metadata instance, with a specified internal source
MetadataInfo::MetadataInfo(MetaBuilder *Parent, UInt32 BodySID, EssenceSourcePtr &Source, WrappingConfigPtr &Config) : Parent(Parent)
{
	// Add a material package and set it as the master
	Master = new PackageInfo(this, true);
	Packages.push_back(Master);

	pMaterialPackage = Master;

	// Add the file package
	pFilePackage = AddFilePackage(BodySID, Source, Config);
}


//! Add a file package to this metadata instance with a single essence track for the specified essence
PackageInfo* MetadataInfo::AddFilePackage(UInt32 BodySID, EssenceSourcePtr &Source, WrappingConfigPtr &Config)
{
	PackageInfo *NewPackage = new PackageInfo(this, BodySID, Source, Config);
	Packages.push_back(NewPackage);

	//! Add tracks that reference all of the tracks in the new package
	if(Master) Master->AddEssenceTracks(NewPackage);

	return NewPackage;
}


//! Add an empty file package to this metadata instance
PackageInfo* MetadataInfo::AddFilePackage(void)
{
	PackageInfo *NewPackage = new PackageInfo(this, false);
	Packages.push_back(NewPackage);

	//! Add tracks that reference all of the tracks in the new package
	if(Master) Master->AddEssenceTracks(NewPackage);

	return NewPackage;
}

//! Add another material package to this metadata instance
PackageInfo* MetadataInfo::AddMaterialPackage(void)
{
	PackageInfo *NewPackage = new PackageInfo(this, true);
	Packages.push_back(NewPackage);

	return NewPackage;
}


//! Add essence tracks that reference all essence tracks in a given package to this packages
void PackageInfo::AddEssenceTracks(PackageInfo *Package)
{
	TrackInfoList::iterator it = Package->EssenceTracks.begin();
	while(it != Package->EssenceTracks.end())
	{
		AddEssenceTrack(*it);
		it++;
	}
}


//! Add an essence track to this package that references a given track (in another package)
TrackInfo* PackageInfo::AddEssenceTrack(TrackInfo *Track)
{
	TrackInfoPtr NewTrack = new TrackInfo(this);
	NewTrack->Type = Track->Type;
	NewTrack->Clips.push_back(new ClipInfo(NewTrack, Track));

	EssenceTracks.push_back(NewTrack);

	return NewTrack;
}


//! Add one or more essence tracks linked to the specified source, limited to a specified number of channels per track
TrackInfoList PackageInfo::AddEssenceMultiTracks(EssenceSource *Source, WrappingConfig *Config, int ChannelsPerTrack /*=1*/)
{
	// FIXME: Should we add sub-sources here too?

	TrackInfoList NewTracks;

	// Add multiple sound tracks
	if(Source->IsSoundEssence())
	{
		UInt32 ChannelCount = Config->EssenceDescriptor->GetUInt(ChannelCount_UL);
		if(ChannelCount == 0)
		{
			error("PackageInfo::AddEssenceMultiTracks() cannot split audio channels from %s - the essence descriptor does not specify the number of channels\n", Source->Name().c_str());
			// Fall through and add a single track
		}

		// If the number of channels is within the requested maximum, add a single track
		if(static_cast<int>(ChannelCount) <= ChannelsPerTrack)
		{
			TrackInfo *Track = AddEssenceTrack(Source, Config);
			if(Track) NewTracks.push_back(Track);
			return NewTracks;
		}

		// Add new tracks, limited to the maximum channel number
		int ChannelsLeft = static_cast<int>(ChannelCount);
		while(ChannelsLeft > 0)
		{
			WrappingConfigPtr SubConfig = new EssenceParser::WrappingConfig(*Config);
			SubConfig->EssenceDescriptor = Config->EssenceDescriptor->MakeCopy();
			SubConfig->EssenceDescriptor->SetInt(ChannelCount_UL, ChannelsLeft > ChannelsPerTrack ? ChannelsPerTrack : ChannelsLeft);

			TrackInfo *Track = AddEssenceTrack(Source, SubConfig);
			if(Track) NewTracks.push_back(Track);

			ChannelsLeft -= ChannelsPerTrack;
		}
	}
	// Add picture and sound tracks for compound essence
	else if(Source->IsCompoundEssence())
	{
		error("PackageInfo::AddEssenceMultiTracks() does not currently support splitting channels from %s\n", Source->Name().c_str());
	}
	else
	{
		error("PackageInfo::AddEssenceMultiTracks() does not currently support splitting channels from %s\n", Source->Name().c_str());
	}

	return NewTracks;
}


//! Constructor for clip referencing another track
ClipInfo::ClipInfo(TrackInfo *Parent, TrackInfo *RefTrack ) : Parent(Parent), RefTrack(RefTrack), AutoDuration(true), InRecursion(false)
{
	Start = 0;
	Duration = RefTrack->GetDuration();
}


//! Constructor for clip referencing another clip
ClipInfo::ClipInfo(TrackInfo *Parent, ClipInfo *RefClip ) : Parent(Parent), AutoDuration(true), InRecursion(false)
{
	RefTrack = RefClip->Parent;
	Start = 0;
	Duration = RefClip->Duration;
}


//! Constructor for clip referencing a specific section of another clip
ClipInfo::ClipInfo(TrackInfo *Parent, ClipInfo *RefClip, Position Start, Length Duration )
				: Parent(Parent), Start(Start), Duration(Duration), AutoDuration(false), InRecursion(false)
{
	RefTrack = RefClip->Parent;
}


//! Constructor with a single internal essence track
PackageInfo::PackageInfo(MetadataInfo *Parent, UInt32 BodySID, EssenceSource *Source, WrappingConfig *Config) : Parent(Parent), IsMaterial(false), BodySID(BodySID), IndexSID(0) 
{
	// Add a new essence track for this essence
	EssenceTracks.push_back(new TrackInfo(this, Source, Config));

	// Set the UMID from the source type
	PackageID = MakeUMID(Source);
}


//! Add an essence track to this package for the given internal essence
TrackInfo* PackageInfo::AddEssenceTrack(EssenceSource *Source, WrappingConfig *Config)
{
	// Add a new essence track for this essence
	TrackInfo *NewTrack = new TrackInfo(this, Source, Config);
	EssenceTracks.push_back(NewTrack);

	// Add link from the master material package (if there is one)
	if(Parent->Master) Parent->Master->AddEssenceTrack(NewTrack);

	return NewTrack;
}


//! Set the details that will be used for building Identification sets
void MetaBuilder::SetIdent(std::string CompanyName, std::string ProductName, std::string VersionString, ProductVersionStruct VersionStruct, UUIDPtr &ProductUID)
{
	this->CompanyName = CompanyName;
	this->ProductName = ProductName;
	this->VersionString = VersionString;
	this->VersionStruct = VersionStruct;
	
	// DRAGONS: We set a local UUID from the value given - this is to prevent changes in the copy help by the caller from changing our copy too
	this->ProductUID.Set(ProductUID->GetValue());
}


//! Build an instance of the metadata for this info block
bool MetadataInfo::BuildInstance(void)
{
	// Check that we aren't trying to re-build an existing instance
	if(Instance)
	{
		error("MetadataInfo::BuildInstance() called for an already build Metadata instance\n");
		return false;
	}

	// Build the empty metadata instance, and record our parent
	Instance = new Metadata();

	/* Build all the packages for this instance */
	
	MDObjectPtr MPackageObject;						// Record the first material package MDObject so we can set it as the primary package
													// TODO: This needs to be updated to allow OP-Atom or OP-nC files with selectable primary package
	PackageInfoList::iterator it = Packages.begin();
	while(it != Packages.end())
	{
		(*it)->UpdateInstance();

		if((*it)->IsMaterial)
		{
			if(!MPackageObject) MPackageObject == (*it)->Instance->Object;
		}

		it++;
	}

	// Set the material package as the primary package
	if(MPackageObject) Instance->MakeRef(PrimaryPackage_UL, MPackageObject);


	// Determine the OP-Label
	SetOPLabel();

	// Set the ident set info
	MDObjectPtr Ident = new MDObject(Identification_UL);
	Ident->SetString(CompanyName_UL, Parent->CompanyName);
	Ident->SetString(ProductName_UL, Parent->ProductName);
	Ident->SetString(VersionString_UL, Parent->VersionString);
	if(!Parent->VersionStruct.IsEmpty()) Ident->SetString(ProductVersion_UL, Parent->VersionStruct.GetString());
	Ident->SetString(ToolkitVersion_UL, LibraryProductVersion());
	Ident->SetString(Platform_UL, "MXFLib (" + OSName() + ")");
	Ident->SetString(ProductUID_UL, Parent->ProductUID.GetString());


	// Finally update generation IDs and add the ident set
	Instance->UpdateGenerations(Ident);


	// All done?
	return true;
}


//! Update the existing instance data
bool MetadataInfo::UpdateInstance(void)
{
	// Check that we have an existing instance
	if(!Instance)
	{
		error("MetadataInfo::UpdateInstance() called without having built a Metadata instance\n");
		return false;
	}

	/* Update all the packages for this instance */

	// Clear the package list in the instance - any updated ones get added back (some may have been deleted)
	Instance->Packages.clear();

	// Update each package and move back into the instance
	PackageInfoList::iterator it = Packages.begin();
	while(it != Packages.end())
	{
		// Update the instance
		(*it)->UpdateInstance();

		// And add it back to the metadata instance
		Instance->Packages.push_back((*it)->Instance);

		it++;
	}

	// TODO: Should we do something here in the way of reporting if there is anything left in the OldPackages list?

	// FIXME: This is where we should update the primary package - in case they changed it


	// Remake the OP-Label
	SetOPLabel();

	// Set the ident set info for the new update
	MDObjectPtr Ident = new MDObject(Identification_UL);
	Ident->SetString(CompanyName_UL, Parent->CompanyName);
	Ident->SetString(ProductName_UL, Parent->ProductName);
	Ident->SetString(VersionString_UL, Parent->VersionString);
	if(!Parent->VersionStruct.IsEmpty()) Ident->SetString(ProductVersion_UL, Parent->VersionStruct.GetString());
	Ident->SetString(ToolkitVersion_UL, LibraryProductVersion());
	Ident->SetString(Platform_UL, "MXFLib (" + OSName() + ")");
	Ident->SetString(ProductUID_UL, Parent->ProductUID.GetString());


	// Finally update generation IDs and add the new ident set
	Instance->UpdateGenerations(Ident, Now2String());


	// All done?
	return true;
}


//! Constructor for essence track with specified source
TrackInfo::TrackInfo(PackageInfo *Parent, EssenceSource *Source, WrappingConfig *Config) 
	: Parent(Parent), TrackNumber(0), TrackID(0), Origin(0), Source(Source), Config(Config), AutoDuration(true), AutoDescriptorDuration(true), InRecursion(false)
{
	// Determine the track type
	if(Source->IsPictureEssence()) Type = Track::TrackTypePictureEssence;
	else if(Source->IsSoundEssence()) Type = Track::TrackTypeSoundEssence;
	else if(Source->IsCompoundEssence()) Type = Track::TrackTypePictureEssence;	// DRAGONS: Only the picture track is added here!
	else if(Source->IsDataEssence()) Type = Track::TrackTypeDataEssence;
	else Type = Track::TrackTypeUnknown;

	Descriptor = Config->EssenceDescriptor;

	// Add an all-encompassing clip
	Clips.push_back(new ClipInfo(this));
}


//! Build or update an instance of the package for this info block
bool PackageInfo::UpdateInstance(void)
{
	// Construct the instance if we have not yet done so
	if(!Instance)
	{
		if(IsMaterial) Instance = Parent->Instance->AddMaterialPackage(PackageID);
		else Instance = Parent->Instance->AddFilePackage(BodySID, PackageID);
	}

	/* Examine the essence tracks to determine a usable edit rate, and the longest track duration */
	Rational PackageEditRate(0,0);
	Length LongestDuration = -1;
	if(!TimecodeTracks.empty())
	{
		TrackInfoList::iterator ET_it = EssenceTracks.begin();
		while(ET_it != EssenceTracks.end())
		{
			if(PackageEditRate.Denominator == 0) PackageEditRate = (*ET_it)->GetEditRate();
			
			Length TrackDuration = (*ET_it)->GetDuration();
			if(TrackDuration > LongestDuration) LongestDuration = TrackDuration;

			ET_it++;
		}
	}

	// Clear the track list in the instance - any updated ones get added back (some may have been deleted)
	Instance->Tracks.clear();

	/* First add any timecode tracks */
	TimecodeTrackInfoList::iterator TT_it = TimecodeTracks.begin();
	while(TT_it != TimecodeTracks.end())
	{
		Rational EditRate;

		// If the edit rate is unknown, determine from essence tracks
		if((*TT_it)->FPS == 0) EditRate = PackageEditRate;
		else
		{
			if((*TT_it)->DropFrame)
			{
				EditRate.Numerator = static_cast<Int32>((*TT_it)->FPS) * 1000;
				EditRate.Denominator = 1001;
			}
			else
			{
				EditRate.Numerator = (*TT_it)->FPS;
				EditRate.Denominator = 1;
			}
		}

		// Work out the start value for this track
		Position TrackStart = (*TT_it)->Start;
		if(!(*TT_it)->StartString.empty())
		{
			if((*TT_it)->FPS)
			{
				Timecode_t StartTC((*TT_it)->StartString.c_str(), (*TT_it)->FPS, (*TT_it)->DropFrame);
				TrackStart = StartTC.Start();
			}
			else
			{
				Timecode_t StartTC((*TT_it)->StartString.c_str());
				TrackStart = StartTC.Start();
			}
		}

		// Check if this is a new timecode track that will need a new instance
		if(!(*TT_it)->Instance)
		{
			// Ensure we allocate the track ID rather than leaving it up to the instance
			if((*TT_it)->TrackID == 0) (*TT_it)->AllocateTrackID();

			(*TT_it)->Instance = Instance->AddTimecodeTrack((*TT_it)->TrackNumber, EditRate, "Timecode Track", (*TT_it)->TrackID);
	
			// TODO: Allow for discontinuous timecode tracks
			(*TT_it)->Instance->AddTimecodeComponent(TrackStart, (*TT_it)->Duration);
		}
		else
		{
			// Update the track edit rate
			MDObjectPtr Ptr = (*TT_it)->Instance->Child(EditRate_UL);
			if(!Ptr) Ptr = (*TT_it)->Instance->AddChild(EditRate_UL);
			Ptr->SetInt("Numerator", EditRate.Numerator);
			Ptr->SetInt("Denominator", EditRate.Denominator);

			// Update the track number and ID
			(*TT_it)->Instance->SetUInt((*TT_it)->TrackNumber);
			(*TT_it)->Instance->SetUInt((*TT_it)->TrackID);

			// TODO: Allow for discontinuous timecode tracks
			(*TT_it)->Instance->Components.front()->SetDuration((*TT_it)->Duration < 0 ? LongestDuration : (*TT_it)->Duration);
			(*TT_it)->Instance->Components.front()->SetInt64(StartTimecode_UL, TrackStart);
		}

		TT_it++;
	}

	/* Next add any essence tracks */
	TrackInfoList::iterator ET_it = EssenceTracks.begin();
	while(ET_it != EssenceTracks.end())
	{
		// Get edit rate for later use
		Rational EditRate = (*ET_it)->GetEditRate();

		TrackPtr ThisTrack = (*ET_it)->Instance;
		if(!ThisTrack)
		{
			// Ensure we allocate the track ID rather than leaving it up to the instance
			if((*ET_it)->TrackID == 0) (*ET_it)->AllocateTrackID();

			switch((*ET_it)->Type)
			{
				case Track::TrackTypePictureEssence: ThisTrack = Instance->AddPictureTrack((*ET_it)->TrackNumber, EditRate, "Picture Track", (*ET_it)->TrackID); break;
				case Track::TrackTypeSoundEssence: ThisTrack = Instance->AddSoundTrack((*ET_it)->TrackNumber, EditRate, "Sound Track", (*ET_it)->TrackID); break;
				case Track::TrackTypeDataEssence: ThisTrack = Instance->AddDataTrack((*ET_it)->TrackNumber, EditRate, "Data Track", (*ET_it)->TrackID); break;
				default: error("Unsupported track type %d in PackageInfo::UpdateInstance()\n", (int)((*ET_it)->Type)); break;
			}
			(*ET_it)->Instance = ThisTrack;

			// If the TrackID is set for auto, read back what was set
			if(ThisTrack && ((*ET_it)->TrackID == 0)) (*ET_it)->TrackID = ThisTrack->GetInt(TrackID_UL);
		}
		else
		{
			// Update the track edit rate - which may have changed
			if(EditRate.Denominator != 0)
			{
				MDObjectPtr Ptr = (*ET_it)->Instance->Child(EditRate_UL);
				if(!Ptr) Ptr = (*ET_it)->Instance->AddChild(EditRate_UL);
				Ptr->SetInt("Numerator", EditRate.Numerator);
				Ptr->SetInt("Denominator", EditRate.Denominator);
			}

			// TODO: Update any other properties that may have changed
		}

		if(ThisTrack)
		{
			// Start with an empty list of source clips - as the structure may have changed a lot
			ThisTrack->Components.clear();

			// If there is no clip in this track - add a sourceclip with an undetermined length
			if((*ET_it)->Clips.empty())
			{
				ThisTrack->AddSourceClip();
			}
			else
			{
				ClipInfoList::iterator C_it = (*ET_it)->Clips.begin();
				while(C_it != (*ET_it)->Clips.end())
				{
					SourceClipPtr Clip = (*C_it)->Instance;
					if(!Clip)
					{
						// Add a new one if it does not yet exist
						Clip = ThisTrack->AddSourceClip((*C_it)->Duration);
						(*C_it)->Instance = Clip;
					}
					else
					{
						// Otherwise, simply update the duration
						Clip->SetDuration((*C_it)->Duration);
					}

					if((*C_it)->RefTrack)
					{
						if((*C_it)->RefTrack->TrackID == 0) (*C_it)->RefTrack->AllocateTrackID();
						Clip->MakeLink((*C_it)->RefTrack->Parent->PackageID, (*C_it)->RefTrack->TrackID, (*C_it)->Start);
					}
					else
					{
						/* Ensure that this is the end of the reference chain */

						static UInt8 NULL_UMID[32] = { 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0 };
						Clip->SetInt64(StartPosition_UL, 0);
						Clip->SetUInt(SourceTrackID_UL, 0);
						Clip->SetValue(SourcePackageID_UL, DataChunk(32, NULL_UMID));
					}

					C_it++;
				}
			}
		}

		ET_it++;
	}

	// Build the essence descriptor from the descriptors for each track (if this is a source package)
	if(!IsMaterial)
	{
		MDObjectPtr Descriptor;
		ET_it = EssenceTracks.begin();
		while(ET_it != EssenceTracks.end())
		{
			if((*ET_it)->Descriptor)
			{
				// If we don't yet have a descriptor...
				if(!Descriptor)
				{
					Descriptor = (*ET_it)->Descriptor;
					
					// Ensure that the TrackID is set correctly
					// DRAGONS: We use a default value for TrackID that will make the comparison fail if there is currently no property
					if(Descriptor->GetUInt(LinkedTrackID_UL, (*ET_it)->TrackID + 1) != (*ET_it)->TrackID)
					{
						Descriptor->SetUInt(LinkedTrackID_UL, (*ET_it)->TrackID);
					}
				}
				else
				{
					if(!Descriptor->IsA(MultipleDescriptor_UL))
					{
						/* We need to build a multiple descriptor, then add the current descriptor, and the new one, to it */

						MDObjectPtr MDesc = new MDObject(MultipleDescriptor_UL);

						// Copy Sample Rate from the current descriptor
						MDesc->SetString(SampleRate_UL, Descriptor->GetString(SampleRate_UL));

						// Set the Essence Container label
						MDesc->AddChild(EssenceContainer_UL,false)->SetValue(DataChunk(16,mxflib::GCMulti_Data));

						// Link the current sub-descriptor as a sub-descriptor
						MDesc->AddRef(FileDescriptors_UL, Descriptor);

						// Use this sub-descriptor as the new decriptor for this package
						Descriptor = MDesc;
					}

				
					/* We have a multiple descriptor - add the descriptor for the new track to it */

					if((*ET_it)->Descriptor->IsA(MultipleDescriptor_UL))
					{
						/* The new track descriptor is a multiple descriptor too */
						
						MDObjectPtr SubDescriptors = (*ET_it)->Descriptor->Child(FileDescriptors_UL);

						if(SubDescriptors)
						{
							MDObject::iterator it = SubDescriptors->begin();
							while(it != SubDescriptors->end())
							{
								if((*it).second->GetRef())
								{
									Descriptor->AddRef(FileDescriptors_UL, (*it).second->GetRef());

									// Ensure that the TrackID is set to this track
									// FIXME: This sets all descriptors of a sub-descriptor to this track!
									// DRAGONS: We use a default value for TrackID that will make the comparison fail if there is currently no property
									if((*it).second->GetRef()->GetUInt(LinkedTrackID_UL, (*ET_it)->TrackID + 1) != (*ET_it)->TrackID)
									{
										(*it).second->GetRef()->SetUInt(LinkedTrackID_UL, (*ET_it)->TrackID);
									}
								}
								it++;
							}
						}
					}
					else
					{
						/* Single descriptor to add to the current multiple descriptor */
						
						Descriptor->AddRef(FileDescriptors_UL, (*ET_it)->Descriptor);

						// Ensure that the TrackID is set correctly
						// DRAGONS: We use a default value for TrackID that will make the comparison fail if there is currently no property
						if((*ET_it)->Descriptor->GetUInt(LinkedTrackID_UL, (*ET_it)->TrackID + 1) != (*ET_it)->TrackID)
						{
							(*ET_it)->Descriptor->SetUInt(LinkedTrackID_UL, (*ET_it)->TrackID);
						}
					}
				}
			}
		
			ET_it++;
		}

		// Link in the descriptor
		if(Descriptor) Instance->MakeRef(Descriptor_UL, Descriptor);
	}

	// Update the package modification time
	Instance->SetString(PackageModifiedDate_UL, Now2String());

	return true;
}


//! Determine and set the Operational Pattern label for this metadata instance
void MetadataInfo::SetOPLabel(void)
{
	if(!Instance)
	{
		error("MetadataInfo::SetOPLabel() called before the instance is built\n");
		return;
	}

	// The base OP-Label to be modified into the required value
	UInt8 OP_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x04, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x02, 0x01, 0xff, 0xff, 0xff, 0x00 };

	/* Determine the OP axes */

	int MPCount = 0;
	size_t HighestMPEssenceTrackCount = 0;
	size_t HighestMPClipCount = 0;
	bool HasExternalEssence = false;
	std::map<UMID, bool> EssencePackageIDs;			// Map of PackageID to true for internal essence, false for external

	// Do all our testing on the material packages
	PackageInfoList::iterator it = Packages.begin();
	while(it != Packages.end())
	{
		if((*it)->IsMaterial)
		{
			MPCount++;

			// Is this the highest essence track count?
			if((*it)->EssenceTracks.size() > HighestMPEssenceTrackCount) HighestMPEssenceTrackCount = (*it)->EssenceTracks.size();
			
			// Check the clips in each essence track
			TrackInfoList::iterator Track_it = (*it)->EssenceTracks.begin();
			while(Track_it != (*it)->EssenceTracks.end())
			{
				// Is this the highest essence clip count?
				if((*Track_it)->Clips.size() > HighestMPClipCount) HighestMPClipCount = (*Track_it)->Clips.size();

				// Record any top level package IDs, and determine if they are in the same file
				ClipInfoList::iterator Clip_it = (*Track_it)->Clips.begin();
				while(Clip_it != (*Track_it)->Clips.end())
				{
					if((*Clip_it)->RefTrack)
					{
						// See if we have already found this UMID
						UMIDPtr &RefPackageID = (*Clip_it)->RefTrack->Parent->PackageID;
						if(RefPackageID)
						{
							std::map<UMID, bool>::iterator Find_it = EssencePackageIDs.find(*RefPackageID);
							if(Find_it == EssencePackageIDs.end())
							{
								// Not yet known - scan packages to find it
								PackageInfoList::iterator Scan_it = Packages.begin();
								while(Scan_it != Packages.end())
								{
									if(!(*Scan_it)->IsMaterial)
									{
										if((*Scan_it)->PackageID)
										{
											// If the UMID matches a file package in this file, then it it internal
											if(*((*Scan_it)->PackageID) == *RefPackageID)
											{
												// Found internal essence if the referenced package has a non-zero BodySID
												if((*Scan_it)->BodySID == 0)
												{
													HasExternalEssence = true;
													EssencePackageIDs[*RefPackageID] = false;
												}
												else
													EssencePackageIDs[*RefPackageID] = true;

												break;
											}
										}
									}

									Scan_it++;
								}

								// If the UMID was not found in any file package, the essence must be external
								if(Scan_it == Packages.end())
								{
									HasExternalEssence = true;
									EssencePackageIDs[*RefPackageID] = false;
								}
							}
						}
					}

					Clip_it++;
				}

				Track_it++;
			}
		}

		it++;
	}


	/* Item Complexity */
	// FIXME: We don't yet determine if it is OP2x or OP3x
	if(HighestMPClipCount == 1) OP_Data[12] = 0x01; else OP_Data[12] = 0x02;

	/* Package Complexity */
	// OPxa and OPxb are limited to a single material package
	if(MPCount == 1)
	{
		// If all references are to a single file package, it is OPxa
		if(EssencePackageIDs.size() == 1) OP_Data[13] = 0x01; else OP_Data[13] = 0x02;
	}
	else
	{
		// Must be OPxc
		OP_Data[13] = 0x03;
	}

	/* Qualifiers */

	// Marker bit
	OP_Data[14] = 0x01;

	// External essence
	if(HasExternalEssence) OP_Data[14] |= 0x02;

	// Stream file
	// FIXME: This needs to be set somehow!

	// Multi-track
	if(HighestMPEssenceTrackCount > 1) OP_Data[14] |= 0x80;

	
	/* Set the actual label */
	ULPtr OPLabel = new UL(OP_Data);
	Instance->SetOP(OPLabel);
}


//! Set the duration of every essence and timecode track to a given value
void MetadataInfo::SetDurations(Length Duration)
{
	// Set the durations in each package
	PackageInfoList::iterator it = Packages.begin();
	while(it != Packages.end())
	{
		(*it)->SetDurations(Duration);
		it++;
	}
//## This is broken!?
}


//! Set the duration of every essence and timecode track to a given value
void PackageInfo::SetDurations(Length Duration)
{
	// Set the durations for each timecode track
	TimecodeTrackInfoList::iterator TT_it = TimecodeTracks.begin();
	while(TT_it != TimecodeTracks.end())
	{
		(*TT_it)->SetDuration(Duration);
		TT_it++;
	}

	// Set the durations for each essence track
	TrackInfoList::iterator ES_it = EssenceTracks.begin();
	while(ES_it != EssenceTracks.end())
	{
		(*ES_it)->SetDuration(Duration);
		ES_it++;
	}
}


//! Set the duration of this track to a given value
void TimecodeTrackInfo::SetDuration(Length Duration, bool MaintainAuto)
{
	this->Duration = Duration;

	if(!MaintainAuto) AutoDuration = false;
}


//! Set the duration of this clip to a given value, allowing the AutoDuration property to be left untouched if required
void ClipInfo::SetDuration(Length Duration, bool MaintainAuto)
{
	this->Duration = Duration;
	if(!MaintainAuto) AutoDuration = false;
}


//! Locate a track for a given track ID
TrackInfo* PackageInfo::LocateTrack(UInt32 TrackID)
{
	TrackInfoList::iterator it = EssenceTracks.begin();
	while(it != EssenceTracks.end())
	{
		if((*it)->TrackID == TrackID) return (*it);
		it++;
	}

	return NULL;
}



//! Determine the edit rate for this track
Rational TrackInfo::GetEditRate(void)
{
	// Check for recursion into ourselves - caused by a loop in the reference chain
	if(InRecursion)
	{
		error("There seems to be a reference loop when a SourceClip ends up referencing itself!\n");
		return Rational(1,1);
	}

	// TODO: If may be possible to make TrackInfo hold a settable edit rate

	// See if we have already set an edit rate in the instance for this track
	if(Instance)
	{
		MDObjectPtr EditRate = Instance->Child(EditRate_UL);
		if(EditRate)
		{
			Rational Ret;
			Ret.Numerator = EditRate->GetInt("Numerator");
			Ret.Denominator = EditRate->GetInt("Denominator");
			return Ret;
		}
	}

	// Next see if we have a source, and use its edit rate
	if(Source) 
	{
		return Source->GetEditRate();
	}
	else
	{
		// Look for a track that is referenced by a clip in this track and get the edit rate from it
		ClipInfoList::iterator it = Clips.begin();
		while(it != Clips.end())
		{
			if((*it)->RefTrack)
			{
				// DRAGONS: Note the recursive loop trap!
				InRecursion = true;
				Rational Ret = (*it)->RefTrack->GetEditRate();
				InRecursion = false;
				return Ret;
			}
			it++;
		}
	}

	return Rational(0,0);
}


//! Set the duration of this clip automatically (if using auto duration)
Length ClipInfo::SetDuration(void)
{
	if(!AutoDuration) return Duration;

	// Check for recursion into ourselves - caused by a loop in the reference chain
	if(InRecursion)
	{
		error("There seems to be a reference loop when a SourceClip ends up referencing itself!\n");
		return Duration;
	}

	/* Try and match the referenced track */
	if(RefTrack)
	{
		// Get the duration of the whole track, auto setting if available
		// DRAGONS: Note the recursive loop trap!
		InRecursion = true;
		Length TrackDuration = RefTrack->SetDuration();
		InRecursion = false;

		// If the referenced track has an unknown duration, so must we
		if(TrackDuration < 0)
		{
			Duration = -1;
			return -1;
		}

		// Reference from the start position to the end of the track
		Duration = TrackDuration - Start;
		return Duration;

		// Unknown track == unknown duration
		Duration = -1;
		return -1;
	}

	/* Match any internal essence */
	if(Parent->Source)
	{
		Duration = Parent->Source->GetCurrentPosition();
	}

	return Duration;
}


//! Set the duration of this track to a given value
void TrackInfo::SetDuration(Length Duration, bool MaintainAuto)
{
	// Remember, then clear, the auto flag
	bool Auto = AutoDuration;
	if(!MaintainAuto) AutoDuration = false;

	/* Before we adjust clip durations, see if there are any auto durations to fix */
	ClipInfoList::iterator it = Clips.begin();
	while(it != Clips.end())
	{
		if((*it)->AutoDuration) (*it)->SetDuration();

		it++;
	}

	// If this is a source package track, with a descriptor and it is in auto-duration mode set the duration in the descriptor
	if((!Parent->IsMaterial) && Descriptor && AutoDescriptorDuration)
	{
		Descriptor->SetInt64(ContainerDuration_UL, Duration);
	}

	// FIXME: We need to use the auto flag!!
	// =====================================

	// We need at least one clip for this to work
	if(Clips.empty()) Clips.push_back(new ClipInfo(this));

	// If we have had a request to set 'unknown' duration, just set the last clip duration to unknown
	if(Duration < 0)
	{
		Clips.back()->Duration = -1;
		return;
	}

	/* Step 1: Check for any clips with undefined lengths and count defined lengths */
	Length CurrentDuration = 0;
	ClipInfoList UndefinedClips;
	it = Clips.begin();
	while(it != Clips.end())
	{
		if((*it)->Duration == -1) UndefinedClips.push_back(*it);
		else CurrentDuration += (*it)->Duration;

		it++;
	}

	// There are some undefined clips - make all adjustments using those
	if(!UndefinedClips.empty())
	{
		if(CurrentDuration > Duration)
		{
			error("TrackInfo::SetDuration() does not currently support shortening tracks with some undefined duration clips\n");
			return;
		}

		// Evenly distribute the number of edit units required between the undefined clips
		size_t UndefinedCount = UndefinedClips.size();
		Length DefinedLength = (Duration - CurrentDuration) / static_cast<Length>(UndefinedCount);

		// Define the length of each undefined clip
		it = UndefinedClips.begin();
		while(it != UndefinedClips.end())
		{
			// Use the last clip to soak up any rounding errors
			if(--UndefinedCount == 0) (*it)->Duration = (Duration - CurrentDuration);
			else
			{
				(*it)->Duration = DefinedLength;
				CurrentDuration += DefinedLength;
			}

			it++;
		}

		return;
	}


	/* Scan through until we find the clip that will hold the end of the track.
	 * This may not be the last one as we could be shortening the track! */
	Position ClipStart = 0;
	it = Clips.begin();
	while(it != Clips.end())
	{
		if((ClipStart + (*it)->Duration) >= Duration)
		{
			/* Found a clip that contains the desired duration */

			// Trim this clip to end at the exact desired track duration
			(*it)->Duration = Duration - ClipStart;

			// Remove any remaining clips (from the next clip to the end, if there is a next clip)
			it++;
			if(it != Clips.end())
			{
				if((*it)->Instance)
				{
					error("Attempted to trim clip from track when instance already built - not yet supported\n");
				}

				Clips.erase(it, Clips.end());
			}

			return;
		}

		it++;
	}

	/* Must have run off the end of the track - so extend the last clip */

	// DRAGONS: We already counted the duration of the last clip in ClipStart, so we must remove it in the calculation
	Clips.back()->Duration = Duration - (ClipStart - Clips.back()->Duration);
}


//! Set the duration of this track automatically (if using auto duration)
Length TrackInfo::SetDuration(void)
{
	Length Ret = 0;

	ClipInfoList::iterator it = Clips.begin();
	while(it != Clips.end())
	{
		// Auto set the duration of any clips in auto mode, just read the duration of others
		Length ClipDuration = (*it)->AutoDuration ? (*it)->SetDuration() : (*it)->Duration;

		// If this clip is 'unknown', the whole track is unknown
		if(ClipDuration < 0) Ret = -1;

		// Otherwise update the total duration
		else if(Ret >= 0) Ret += ClipDuration;

		it++;
	}

	// If this is a source package track, with a descriptor and it is in auto-duration mode set the duration in the descriptor
	if((!Parent->IsMaterial) && Descriptor && AutoDescriptorDuration)
	{
		Descriptor->SetInt64(ContainerDuration_UL, Ret);
	}

	return Ret;
}


//! Get the duration of this track from the clips
Length TrackInfo::GetDuration(void)
{
	Length Ret = 0;

	ClipInfoList::iterator it = Clips.begin();
	while(it != Clips.end())
	{
		// If this clip is 'unknown', the whole track is unknown
		if((*it)->Duration < 0) Ret = -1;

		// Otherwise update the total duration
		else if(Ret >= 0) Ret += (*it)->Duration;

		it++;
	}

	return Ret;
}


//! Auto set the duration of every essence and timecode track in this instance
void MetadataInfo::SetDurations(void)
{
	PackageInfoList::iterator it = Packages.begin();
	while(it != Packages.end())
	{
		(*it)->SetDurations();
		it++;
	}
}

//! Auto set the duration of every essence and timecode track in this package
void PackageInfo::SetDurations(void)
{
	Length LongestDuration = -1;

	// Set the essence durations first
	TrackInfoList::iterator ES_it = EssenceTracks.begin();
	while(ES_it != EssenceTracks.end())
	{
		Length ThisDuration = (*ES_it)->SetDuration();
		if(ThisDuration > LongestDuration) LongestDuration = ThisDuration;

		ES_it++;
	}

	// Set the timecode tracks
	TimecodeTrackInfoList::iterator TT_it = TimecodeTracks.begin();
	while(TT_it != TimecodeTracks.end())
	{
		if((*TT_it)->AutoDuration) (*TT_it)->SetDuration(LongestDuration, true);

		TT_it++;
	}
}


//! Build a metadata info block from an existing Metadata instance
MetadataInfoPtr MetadataInfo::Parse(MetaBuilder *Parent, Metadata *Instance)
{
	// Sanity Check
	if((!Parent) || (!Instance)) return NULL;

	// Build the new info block
	MetadataInfoPtr Ret = new MetadataInfo(Parent);

	// Parse each package
	PackageList::iterator it = Instance->Packages.begin();
	while(it != Instance->Packages.end())
	{
		PackageInfoPtr Package = PackageInfo::Parse(Ret, (*it));
		if(Package) Ret->Packages.push_back(Package);

		it++;
	}

	/* Work out if we have a material package that we should treat as the 'master' */
	MDObjectPtr PrimaryPackage = Instance->GetRef(PrimaryPackage_UL);
	if(PrimaryPackage)
	{
		MDObjectPtr PrimaryID = PrimaryPackage->Child(PackageUID_UL);
		if(PrimaryID)
		{
			// Read the primary package ID
			DataChunkPtr Buffer = PrimaryPackage->PutData();
			if(Buffer && (Buffer->Size == 32))
			{
				UMIDPtr PrimaryID = new UMID(Buffer->Data);

				// Search for a material package with that ID
				PackageInfoList::iterator it = Ret->Packages.begin();
				while(it != Ret->Packages.end())
				{
					if(*(*it)->PackageID == *PrimaryID)
					{
						if((*it)->IsMaterial) Ret->Master = (*it);

						// FIXME: We should really store the primary value for later re-use

						break;
					}
					it++;
				}
			}
		}
	}

	/* Parse the EssenceContainerData sets to work out BodySID and IndexSID values */
	// TODO: This is not very efficient - perhaps build a map to speed it up
	MDObjectPtr EssenceContainers = Instance->Child(EssenceContainers_UL);
	if(!EssenceContainers)
	{
		error("Missing EssenceContainers batch in %s at 0x%s\n", Instance->FullName().c_str(), Int64toHexString(Instance->GetLocation(), 8).c_str());
	}
	else
	{
		MDObject::iterator it = EssenceContainers->begin();
		while(it != EssenceContainers->end())
		{
			MDObjectPtr ECSet = (*it).second->GetRef();
			if(!ECSet)
			{
				error("Broken link in %s batch at 0x%s\n", EssenceContainers->FullName().c_str(), Int64toHexString(EssenceContainers->GetLocation(), 8).c_str());
			}
			else
			{
				/* Locate the matching package */
				MDObjectPtr LinkedPackageUID = ECSet->Child(LinkedPackageUID_UL);
				if(!LinkedPackageUID)
				{
					error("No LinkedPackageUID property in %s at 0x%s\n", ECSet->FullName().c_str(), Int64toHexString(ECSet->GetLocation(), 8).c_str());
				}
				else
				{
					UMIDPtr PackageID = new UMID(LinkedPackageUID->GetString());
					PackageInfoList::iterator it = Ret->Packages.begin();
					while(it != Ret->Packages.end())
					{
						if(*PackageID == *((*it)->PackageID))
						{
							// Found a matching package - set the SIDs and stop looking
							(*it)->BodySID = ECSet->GetUInt(BodySID_UL);
							(*it)->IndexSID = ECSet->GetUInt(IndexSID_UL);
							break;
						}
						it++;
					}
					if(it == Ret->Packages.end())
					{
						error("Failed to locate package %s\n", LinkedPackageUID->GetString().c_str());
					}
				}
			}

			it++;
		}
	}

	return Ret;
}



//! Build a package info block from an existing package instance
PackageInfoPtr PackageInfo::Parse(MetadataInfo *Parent, Package *Instance)
{
	// Sanity Check
	if((!Parent) || (!Instance)) return NULL;

	// Build the new info block
	PackageInfoPtr Ret = new PackageInfo(Parent, Instance->IsA(MaterialPackage_UL));

	MDObjectPtr PackageID = Instance->Child(PackageUID_UL);
	if(!PackageID)
	{
		error("No PackageUID property in %s at 0x%s\n", Instance->FullName().c_str(), Int64toHexString(Instance->GetLocation(), 8).c_str());
	}
	else
	{
		DataChunkPtr UMIDBytes = PackageID->PutData();
		if(UMIDBytes->Size != 32)
		{
			error("Unable to parse %s at 0x%s\n", PackageID->FullName().c_str(), Int64toHexString(PackageID->GetLocation(), 8).c_str());
		}
		else
		{
			Ret->PackageID = new UMID(UMIDBytes->Data);
		}
	}

	/* Build a map of descriptors indexed by TrackID - so they can be added to the essence tracks */
	std::map<UInt32, MDObjectPtr> DescriptorMap;
	MDObjectList Descriptors;
	if(!Ret->IsMaterial)
	{
		MDObjectPtr Descriptor = Instance->GetRef(Descriptor_UL);
		
		// TODO: Most likely, a source package without a descriptor is lower-level - should we test/flag this?
		if(Descriptor)
		{
			UInt32 TrackID = Descriptor->GetUInt(LinkedTrackID_UL);
			if(Descriptor->IsA(MultipleDescriptor_UL))
			{
				if(TrackID != 0)
				{
					warning("%s at 0x%s has a LinkedTrackID of %u (Should be zero in a MultipleDescriptor)\n", 
							Descriptor->FullName().c_str(), Int64toHexString(Descriptor->GetLocation(), 8).c_str(), TrackID);
				}
				
				MDObject::iterator it = Descriptor->begin();
				while(it != Descriptor->end())
				{
					Descriptors.push_back((*it).second);
					TrackID = (*it).second->GetUInt(LinkedTrackID_UL);
					if(TrackID == 0)
					{
						if((*it).second->Child(LinkedTrackID_UL))
						{
							error("%s at 0x%s has a LinkedTrackID of zero which is not a valid TrackID\n", (*it).second->FullName().c_str(), Int64toHexString((*it).second->GetLocation(), 8).c_str());
						}
						else
						{
							warning("%s at 0x%s has no LinkedTrackID - will attempt to infer linkage from the order of descriptors\n", (*it).second->FullName().c_str(), Int64toHexString((*it).second->GetLocation(), 8).c_str());
						}
					}
					else
					{
						std::map<UInt32, MDObjectPtr>::iterator Find_it = DescriptorMap.find(TrackID);
						if(Find_it != DescriptorMap.end())
						{
							error("%s at 0x%s is the second descriptor with the LinkedTrackID value of %u for this Source Package - multiple descriptors per track are not yet supported\n",
								  (*it).second->FullName().c_str(), Int64toHexString((*it).second->GetLocation(), 8).c_str(), TrackID);
						}
						else
						{
							DescriptorMap[TrackID] = (*it).second;
						}
					}

					it++;
				}
			}
			else
			{
				Descriptors.push_back(Descriptor);
				if(TrackID != 0) DescriptorMap[TrackID] = Descriptor;
			}
		}
	}


	/* Parse each track */
	TrackList::iterator it = Instance->Tracks.begin();
	while(it != Instance->Tracks.end())
	{
		if((*it)->IsTimecodeTrack())
		{
			TimecodeTrackInfoPtr Track = TimecodeTrackInfo::Parse(Ret, *it);
			if(Track) Ret->TimecodeTracks.push_back(Track);
		}
		else if((*it)->IsEssenceTrack())
		{
			TrackInfoPtr Track = TrackInfo::Parse(Ret, *it);
			if(Track) Ret->EssenceTracks.push_back(Track);
		}
		else
		{
			error("Track type not supported for %s at 0x%s\n", (*it)->FullName().c_str(), Int64toHexString((*it)->GetLocation(), 8).c_str());
		}

		it++;
	}

	return Ret;
}


//! Build a track info block from an existing track instance
TrackInfoPtr TrackInfo::Parse(PackageInfo *Parent, Track *Instance)
{
	// Sanity Check
	if((!Parent) || (!Instance)) return NULL;

	// Build the new info block
	TrackInfoPtr Ret = new TrackInfo(Parent);

	// Determine the type of this track
	Ret->Type = Instance->GetTrackType();
	if(Ret->Type == Track::TrackTypeTimecode)
	{
		error("Timecode track passed to TrackInfo::Parse()\n");
		return NULL;
	}

	// Parse other properties
	Ret->TrackNumber = Instance->GetUInt(TrackNumber_UL);
	Ret->TrackID = Instance->GetUInt(TrackID_UL);
	Ret->Origin = Instance->GetInt64(Origin_UL);

	
	/* Parse the SourceClips for this track */
	// FIXME: Do this!

	return Ret;
}


//! Build a timecode track info block from an existing track instance
TimecodeTrackInfoPtr TimecodeTrackInfo::Parse(PackageInfo *Parent, Track *Instance)
{
	// Sanity Check
	if((!Parent) || (!Instance)) return NULL;

	if(!Instance->IsTimecodeTrack())
	{
		error("Non-Timecode track passed to TimecodeTrackInfo::Parse()\n");
		return NULL;
	}

	// Build the new info block
	TimecodeTrackInfoPtr Ret = new TimecodeTrackInfo(Parent);

	// Parse other properties
	Ret->TrackNumber = Instance->GetUInt(TrackNumber_UL);
	Ret->TrackID = Instance->GetUInt(TrackID_UL);
	
	// Extract properties from TimecodeComponent
	if(Instance->Components.size() == 0)
	{
		error("Timecode track at 0x%s has no TimecodeComponents\n", Instance->FullName().c_str(), Int64toHexString(Instance->GetLocation(), 8).c_str());
		return NULL;
	}

	if(Instance->Components.size() > 1)
	{
		error("Timecode track at 0x%s has %d TimecodeComponents - TimecodeTrackInfo::Parse() currently does not support discontinuous timecodes\n",
			  Instance->FullName().c_str(), Int64toHexString(Instance->GetLocation(), 8).c_str());
	}

	Ret->FPS = Instance->Components.front()->GetUInt(RoundedTimecodeBase_UL);
	if(Instance->Components.front()->GetInt(DropFrame_UL) == 0) Ret->DropFrame = false; else Ret->DropFrame = true;
	Ret->Start = Instance->Components.front()->GetInt64(StartTimecode_UL);

	Ret->Duration = Instance->GetDuration();

	// FIXME: Sort discontinuous timecode tracks

	return Ret;
}


//! Allocate a track ID to this timecode track
UInt32 TimecodeTrackInfo::AllocateTrackID(void)
{
	if(Parent && (TrackID == 0)) Parent->AllocateTrackID(this);
	return TrackID;
}


//! Allocate a track ID to this track
UInt32 TrackInfo::AllocateTrackID(void)
{
	if(Parent && (TrackID == 0)) Parent->AllocateTrackID(this);
	return TrackID;
}


//! Allocate a track ID to a given timecode track
UInt32 PackageInfo::AllocateTrackID(TimecodeTrackInfo *Track)
{
	// TODO: Use a beter method of finding the next free TrackID!

	UInt32 Ret = 1;

	// Make the new value higher than any timecode track ID
	TimecodeTrackInfoList::iterator TT_it = TimecodeTracks.begin();
	while(TT_it != TimecodeTracks.end())
	{
		if(Ret <= (*TT_it)->TrackID) Ret = (*TT_it)->TrackID + 1;
		TT_it++;
	}

	// Make the new value higher than any essence track ID
	TrackInfoList::iterator ET_it = EssenceTracks.begin();
	while(ET_it != EssenceTracks.end())
	{
		if(Ret <= (*ET_it)->TrackID) Ret = (*ET_it)->TrackID + 1;
		ET_it++;
	}

	Track->TrackID = Ret;

	return Ret;
}


//! Allocate a track ID to a given track
UInt32 PackageInfo::AllocateTrackID(TrackInfo *Track)
{
	// TODO: Use a beter method of finding the next free TrackID!

	UInt32 Ret = 1;

	// Make the new value higher than any timecode track ID
	TimecodeTrackInfoList::iterator TT_it = TimecodeTracks.begin();
	while(TT_it != TimecodeTracks.end())
	{
		if(Ret <= (*TT_it)->TrackID) Ret = (*TT_it)->TrackID + 1;
		TT_it++;
	}

	// Make the new value higher than any essence track ID
	TrackInfoList::iterator ET_it = EssenceTracks.begin();
	while(ET_it != EssenceTracks.end())
	{
		if(Ret <= (*ET_it)->TrackID) Ret = (*ET_it)->TrackID + 1;
		ET_it++;
	}

	Track->TrackID = Ret;

	return Ret;
}