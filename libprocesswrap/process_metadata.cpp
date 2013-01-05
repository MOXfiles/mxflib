/*! \file	process_metadata.cpp
 *	\brief	MXF wrapping functions
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

#include "process_metadata.h"

#include <stdio.h>
#include <iostream>
#include <string>
using namespace std;

#include "mxflib/mxflib.h"
using namespace mxflib;

// Include the timecode class
#include "utility/timecode.h"


#include "libprocesswrap/process.h"
#include "process_utils.h"


extern FILE * hLogout; //file handle to send all the informational output to

// Example dark metadata
namespace mxflib
{
	const UInt8 ProMPEGForum_Dark_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x05, 0x0d, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 };
	const UL ProMPEGForum_Dark_UL(ProMPEGForum_Dark_UL_Data);
	ULPtr ProMPEGForum_Dark_ULPtr = new UL( ProMPEGForum_Dark_UL_Data );
}



void ProcessMetadata(int OutFileNum,
					 ProcessOptions    *pOpt,
					 EssenceSourcePair *Source,
					 EssenceParser::WrappingConfigList WrapCfgList,
					 Rational		  EditRate,
					 BodyWriterPtr     Writer,
					 MetadataPtr       MData,
					 UMIDPtr			  MPUMID,
					 UMIDPtr			 *FPUMID,
					 UMIDPtr			 *SPUMID,
					 EssenceStreamInfo*EssStrInf,
					 PackagePtr           &FilePackage,  //OUT variable
					 TimecodeComponentPtr &MPTimecodeComponent //OUT variable
					 )
{

	// Avid special

	MDObjectPtr ContainerDef_MXF;

#if defined( NeedAvidCodecDef )
	// Not needed since Xpress Pro HD 5.
	MDObjectPtr CodecJPEG;
#endif



	EssenceParser::WrappingConfigList::iterator WrapCfgList_it;
	// Set the OP label
	// If we are writing OP-Atom we write the header as OP1a initially as another process
	// may try to read the file before it is complete and then it will NOT be a valid OP-Atom file
	// DRAGONS: This should be OPxx which matches the number of file packages...
	if(pOpt->OPAtom) MData->SetOP(OP1aUL); else MData->SetOP(pOpt->OPUL);

	// Infer dropframe from the edit rate
	bool DropFrame = false;

	// Work out the edit rate from the descriptor
	UInt32 FrameRate = EditRate.Numerator;

	// Use drop-frame for any non-integer frame rate
	if(EditRate.Denominator > 1)
	{
		// This is an integer equivalent of FrameRate = floor((FrameRate + 0.5) / Denominator)
		FrameRate += EditRate.Denominator - 1;
		FrameRate /= EditRate.Denominator;

		DropFrame = true;
	}


	// Build the Material Package
	PackagePtr MaterialPackage;

	MaterialPackage = MData->AddMaterialPackage(pOpt->MobName, MPUMID);


	// OP-Atom requires Primary Package, other OPs don't mandate
	MData->SetPrimaryPackage(MaterialPackage);		// This will be overwritten for OP-Atom


	/*	if(pOpt->ExpOption)
	{
	MDObjectPtr PackageMarkerObject = new MDObject(PackageMarkerObject_UL);
	PackageMarkerObject->SetUInt(TimebaseReferenceTrackID_UL, 1);

	MaterialPackage->MakeRef(PackageMarker_UL, PackageMarkerObject);
	}
	*/





	// Add a Timecode Track
	TrackPtr MPTimecodeTrack;
	if(pOpt->PutTCTrack)
		MPTimecodeTrack= MaterialPackage->AddTimecodeTrack(EditRate);

	// Add a single Timecode Component
	if(pOpt->PutTCTrack)
	{
		Position StartTimecode = TCtoFrames( FrameRate, DropFrame, 1, 0, 0, 0 );
		MPTimecodeComponent= MPTimecodeTrack->AddTimecodeComponent(FrameRate, DropFrame, StartTimecode);
	}

	// Set the writer's general parameters
	Writer->SetKAG(pOpt->KAGSize);
	Writer->SetForceBER4(true);

	if(pOpt->OPAtom && (!pOpt->OPAtom2Part))
	{
		// SMPTE 390M does not recommend placing Essence in the header partition
		// Index data can't share with metadata if very-isolated, essence can never share with metadata
		Writer->SetMetadataSharing(!pOpt->VeryIsolatedIndex, false);
	}
	else
	{
		// Index data can't share with metadata if very-isolated but essence can always share with metadata
		Writer->SetMetadataSharing(!pOpt->VeryIsolatedIndex, !pOpt->IsolatedEssence);
	}








	/* Add essence streams to the writer */ 
	/* ================================= */
	/* These are all added before building the metadata tracks so that the track numbers have complete stream counts */
	/* DRAGONS: This code MUST be kept in step with the logic of the following loop */

	int PreviousFP = -1;							// The index of the previous file package used - allows us to know if we treat this is a sub-stream
	int iStream = -1;								// Stream index (note that it will be incremented to 0 in the first iteration)
	int iTrack = 0;									// Track index
	WrapCfgList_it = WrapCfgList.begin();
	while(WrapCfgList_it != WrapCfgList.end())
	{
		// Move on to a new stream if we are starting a new file package
		if(Source[iTrack].first != PreviousFP) iStream++;

		// Write File Packages except for externally ref'ed essence in OP-Atom
		bool WriteFP = (!pOpt->OPAtom) || (iStream == OutFileNum);

		if(WriteFP && (!(*WrapCfgList_it)->IsExternal)) // (iTrack == OutFileNum)
		{
			// DRAGONS: Always write a track if writing avid atom files, and this is the file holding the essence for the current track
			if((Source[iTrack].first != PreviousFP)
				)
			{
				// Build a stream object to write
				EssStrInf[iStream].Stream = new BodyStream(iStream + 1, Source[iTrack].second);
				SetStreamWrapType(EssStrInf[iStream].Stream, (*WrapCfgList_it)->WrapOpt->ThisWrapType);


				// Add this stream to the body writer
				Writer->AddStream(EssStrInf[iStream].Stream);
			}
			else
			{
				EssStrInf[iStream].Stream->AddSubStream(Source[iTrack].second);
			}

			// The source will be allocated a streamID when it is added to the BodyStream - we need that for track linking later
			EssStrInf[iTrack].EssenceID = Source[iTrack].second->GetStreamID();

			// Ensure that this stream gets zero-padded beyond the end of essence, if requested
			if(pOpt->ZeroPad) Source[iTrack].second->SetOption("EndPadding", 1);
		}

		// Record the file package index used this time
		PreviousFP = Source[iTrack].first;

		// Increment the track
		WrapCfgList_it++;
		iTrack++;
	}


	/* Build the File Packages and all essence tracks */
	/* ============================================== */
	/* DRAGONS: This code MUST be kept in step with the logic of the above loop */

	PreviousFP = -1;								// The index of the previous file package used - allows us to know if we treat this is a sub-stream
	iStream = -1;									// Stream index (note that it will be incremented to 0 in the first iteration)
	iTrack = 0;										// Track index
	int AudioTrackIndex = 0;						// Number for identifying audio tracks
	WrapCfgList_it = WrapCfgList.begin();
	while(WrapCfgList_it != WrapCfgList.end())
	{

		TrackPtr FPTimecodeTrack;

		// Move on to a new stream if we are starting a new file package
		if(Source[iTrack].first != PreviousFP) iStream++;

		// Write File Packages except for externally ref'ed essence in OP-Atom
		bool WriteFP = (!pOpt->OPAtom) || (iStream == OutFileNum);

		if(WriteFP) // (iTrack == OutFileNum)
		{
			if(Source[iTrack].first != PreviousFP)
			{
				/* ====================================================================================================== */
				/* DRAGONS: This is the point that must duplicate the conditions for iStream and iTrack in the loop above */
				/* ====================================================================================================== */

				if(!(*WrapCfgList_it)->IsExternal)
				{
					// Force edit-unit align if requested
					if(pOpt->EditAlign) EssStrInf[iStream].Stream->SetEditAlign(true);

					// Set indexing options for this stream
					// FIXME: This will not work for CBR streams with sub-streams
					if(pOpt->UseIndex || pOpt->SparseIndex || pOpt->SprinkledIndex)
					{
						if((*WrapCfgList_it)->WrapOpt->CBRIndex && (EssStrInf[iStream].Stream->GetSource()->GetBytesPerEditUnit() != 0)) 
						{
							SetStreamIndex(EssStrInf[iStream].Stream, true, pOpt); 
						}
						else
						{
							if((*WrapCfgList_it)->WrapOpt->ThisWrapType == WrappingOption::Frame)
							{
								SetStreamIndex(EssStrInf[iStream].Stream, false, pOpt);
							}
							else
							{
								// Attempt to force the parser to use VBR indexing mode (i.e. return each edit unit individually)
								if(Source[iTrack].second->EnableVBRIndexMode())
								{
									SetStreamIndex(EssStrInf[iStream].Stream, false, pOpt);
								}
								else
								{
									if((*WrapCfgList_it)->WrapOpt->ThisWrapType == WrappingOption::Clip)
										warning("VBR Indexing not supported by \"%s\" when clip-wrapping\n", Source[iTrack].second->Name().c_str());
									else
										warning("VBR Indexing not supported by \"%s\" for the selected wrapping mode\n", Source[iTrack].second->Name().c_str());
								}
							}
						}
					}
				}

				// Add the file package
				UInt32 BodySID = (*WrapCfgList_it)->IsExternal ? 0 : iStream+1;

				std::string FPName("File Package: ");

				FPName += (*WrapCfgList_it)->WrapOpt->Description;
				FilePackage = MData->AddFilePackage(BodySID, FPName, FPUMID[iStream]);


				// Add a timecode track if requested
				if(pOpt->PutTCTrack)
				{
					FPTimecodeTrack = FilePackage->AddTimecodeTrack(EditRate);

					Position StartTimecode = TCtoFrames( FrameRate, DropFrame, 1, 0, 0, 0 );


					EssStrInf[iTrack].FPTimecodeComponent = FPTimecodeTrack->AddTimecodeComponent(FrameRate, DropFrame, StartTimecode );
				}
			}
		}

		Rational SamplingRate = EditRate;

		//! The current descriptor
		MDObjectPtr ThisDescriptor = (*WrapCfgList_it)->EssenceDescriptor;

		//! The sub-descriptor batch, if this is a multiple stream, else NULL
		MDObjectPtr SubDescriptors;

		//! Iterator for scanning the sub-descriptor batch, if this is a multiple stream
		// DRAGONS: Here we initialize the iterator with a generic MDObject::iterator::end() value that won't
		//          ever be used, but keeps strict compilers from complaining it is uninitialized
		MDObject::iterator SubDescriptorsIt = ThisDescriptor->end();

		//! The index number of the sub track within this multiple essence stream, zero for the first
		size_t SubTrackIndex = 0;

		//! Is this essense stream a multiplexed type
		if((*WrapCfgList_it)->EssenceDescriptor->IsA(MultipleDescriptor_UL))
		{
			SubDescriptors = ThisDescriptor[FileDescriptors_UL];
			if(SubDescriptors)
			{
				SubDescriptorsIt = SubDescriptors->begin();
			}
		}

		for(;;)
		{
			// Locate the descriptor if this is a multiple essence stream
			if(SubDescriptors)
			{
				// All done?
				if(SubDescriptorsIt == SubDescriptors->end()) break;

				ThisDescriptor = (*SubDescriptorsIt).second->GetLink();
			
				if(!ThisDescriptor)
				{
					error("Broken link in multiple descriptor for %s\n", (*WrapCfgList_it)->WrapOpt->Description.c_str());
					SubDescriptorsIt++;

					// DRAGONS: We don't increment SubTrackIndex here as we have not added any tracks for this sub-stream
					continue;
				}
			}

			// Add the appropriate Track to the Material Package (if this track is required on the material package)
			bool WriteMP = false;
			if((iStream == OutFileNum) && (iStream < pOpt->InFileGangSize)) WriteMP = true;


			/* Determine what type of tracks to add */
			
			// Use track number 0 for external essence
			UInt32 TrackID = 0;
			
			if( !(*WrapCfgList_it)->IsExternal && EssStrInf[iStream].Stream )
			{
				TrackID = EssStrInf[iStream].Stream->GetTrackNumber(Source[iTrack].second->GetStreamID());
			}


			TrackPtr MPTrack;
			TrackPtr FPTrack;
			if(ThisDescriptor->IsA(GenericPictureEssenceDescriptor_UL))
			{
				if(WriteMP) MPTrack = MaterialPackage->AddPictureTrack(1, EditRate, "V1");
				if(WriteFP) FPTrack = FilePackage->AddPictureTrack(TrackID, EditRate, "V1");
			}
			else
			if(ThisDescriptor->IsA(GenericSoundEssenceDescriptor_UL))
			{
				{
					std::string TrackName="A";
					TrackName+=Int64toString(++AudioTrackIndex);
					if(WriteMP) MPTrack = MaterialPackage->AddSoundTrack(AudioTrackIndex, EditRate,TrackName);
					if(WriteFP)
					{
							FPTrack = FilePackage->AddSoundTrack(TrackID, SamplingRate, TrackName);
					}
				}
			}
			else
			if(ThisDescriptor->IsA(GenericDataEssenceDescriptor_UL))
			{
				if(WriteMP) MPTrack = MaterialPackage->AddDataTrack(EditRate);
				if(WriteFP) FPTrack = FilePackage->AddDataTrack(TrackID, EditRate);
			}
			else
			{
				switch((*WrapCfgList_it)->WrapOpt->GCEssenceType)
				{
				case 0x18:	// Make DV compound essence in to picture tracks
				case 0x05: case 0x15: 
					if(WriteMP) MPTrack = MaterialPackage->AddPictureTrack(1, EditRate, "V1");
					if(WriteFP) FPTrack = FilePackage->AddPictureTrack(TrackID, EditRate, "V1");
					break;
				case 0x06: case 0x16:
					{
						std::string TrackName="A";
						TrackName+=Int64toString(++AudioTrackIndex);
						if(WriteMP) MPTrack = MaterialPackage->AddSoundTrack(iTrack,EditRate,TrackName);
						if(WriteFP)
						{
								FPTrack = FilePackage->AddSoundTrack(TrackID, SamplingRate, TrackName);
						}
					}
					break;
				case 0x07: case 0x17: default:
					if(WriteMP) MPTrack = MaterialPackage->AddDataTrack(EditRate);
					if(WriteFP) FPTrack = FilePackage->AddDataTrack(TrackID, EditRate);
					break;
				}
			}

			/* Add these new tracks to the essence info struct */
			if(MPTrack)
			{
				if(EssStrInf[iTrack].MPTrack) EssStrInf[iTrack].MPSubTracks.push_back(MPTrack);
				else EssStrInf[iTrack].MPTrack = MPTrack;
			}
			if(FPTrack)
			{
				if(EssStrInf[iTrack].FPTrack) EssStrInf[iTrack].FPSubTracks.push_back(FPTrack);
				else EssStrInf[iTrack].FPTrack = FPTrack;
			}

			// Exit loop after a single iteration if not multiple essence
			if(!SubDescriptors) break;

			// Move to next sub-descriptor
			SubDescriptorsIt++;
			SubTrackIndex++;
		}

		// Locate the material package track this essence is in
		int TrackNumber = iTrack;
		while(TrackNumber >= pOpt->InFileGangSize) TrackNumber -= pOpt->InFileGangSize;

		if((TrackNumber != iTrack) && (!EssStrInf[TrackNumber].MPSubTracks.empty()))
		{
			error("Cannot currently build higher-OPs with multiple essence tracks per source - file may be invalid");
		}

		// Add a single Component to this Track of the Material Package
		int STID = -1;

		if(EssStrInf[TrackNumber].MPTrack)
		{
			EssStrInf[iTrack].MPClip = EssStrInf[TrackNumber].MPTrack->AddSourceClip();
			STID=EssStrInf[iTrack].MPClip[SourceTrackID_UL]->GetInt();
		}

		// Add a single Component to this Track of the File Package
		if(WriteFP)
		{
				EssStrInf[iTrack].FPClip = SmartPtr_Cast( EssStrInf[iTrack].FPTrack->AddSourceClip(), Component );

				// Add empty (terminating) SourceClips to each FP sub-track
				if(!EssStrInf[iTrack].FPSubTracks.empty())
				{
					TrackList::iterator f_it = EssStrInf[iTrack].FPSubTracks.begin();
					while(f_it != EssStrInf[iTrack].FPSubTracks.end())
					{
						(*f_it)->AddSourceClip();
						f_it++;
					}
				}
		}





		// Add the file descriptor to the file package
		// except for externally ref'ed essence in OP-Atom
		if( pOpt->OPAtom )
		{
			// Write a File Descriptor only on the internally ref'ed Track 
			if( WriteFP ) // (iTrack == OutFileNum)
			{
				MDObjectPtr Descriptor=(*WrapCfgList_it)->EssenceDescriptor;

				FilePackage->AddChild(Descriptor_UL)->MakeLink(Descriptor);



				MData->AddEssenceType((*WrapCfgList_it)->WrapOpt->WrappingUL);

				if((*WrapCfgList_it)->EssenceDescriptor->IsA(MultipleDescriptor_UL))
				{
					// Ensure that we have flagged a multiple descriptor if one is used
					ULPtr GCUL = new UL( mxflib::GCMulti_Data );
					MData->AddEssenceType( GCUL );
				}

				// Link the MP to the FP
				if(EssStrInf[iTrack].MPClip) EssStrInf[iTrack].MPClip->MakeLink(EssStrInf[iTrack].FPTrack, 0);
			}
			else // (!WriteFP)
			{
				// Link the MP to the external FP
				// DRAGONS: We must assume what the linked track will be... track 1 picked as that is what is needed for OP atom files
				if(EssStrInf[iTrack].MPClip) EssStrInf[iTrack].MPClip->MakeLink(FPUMID[iTrack], 1, 0);
			}
		}
		else if( pOpt->FrameGroup ) // !pOpt->OPAtom
		{
			if(WriteFP)
			{
				MDObjectPtr MuxDescriptor = FilePackage->GetRef(Descriptor_UL);

				// Write a MultipleDescriptor only on the first Iteration
				if( !MuxDescriptor )
				{
					MuxDescriptor = new MDObject(MultipleDescriptor_UL);
					MuxDescriptor->AddChild(SampleRate_UL)->SetInt("Numerator",(*WrapCfgList_it)->EssenceDescriptor[SampleRate_UL]->GetInt("Numerator"));
					MuxDescriptor->AddChild(SampleRate_UL)->SetInt("Denominator",(*WrapCfgList_it)->EssenceDescriptor[SampleRate_UL]->GetInt("Denominator"));

					MuxDescriptor->AddChild(EssenceContainer_UL,false)->SetValue(DataChunk(16,mxflib::GCMulti_Data));

					MuxDescriptor->AddChild(FileDescriptors_UL);
					FilePackage->AddChild(Descriptor_UL)->MakeLink(MuxDescriptor);
				}

				if((*WrapCfgList_it)->EssenceDescriptor->IsA(MultipleDescriptor_UL))
				{
					// Copy over all existing sub-descriptors from this new multiple descriptor
					MDObjectPtr SubDesc = (*WrapCfgList_it)->EssenceDescriptor->Child(FileDescriptors_UL);
					if(SubDesc)
					{
						TrackList::iterator SubTrack_it = EssStrInf[iTrack].FPSubTracks.begin();
						MDObject::iterator it = SubDesc->begin();
						while(it != SubDesc->end())
						{
							MDObjectPtr ThisDesc = (*it).second->GetRef();
							if(ThisDesc)
							{
								// Add the sub-descriptor
								MuxDescriptor->AddRef(FileDescriptors_UL, ThisDesc);

								// Work out the TrackID for each sub-descriptor
								if(it == SubDesc->begin())
								{
									// The first stream will have ended up ad the master FPTrack
									ThisDesc->SetUInt(LinkedTrackID_UL, EssStrInf[iTrack].FPTrack->GetUInt(TrackID_UL));
								}
								else
								{
									// Each further stream will be a sub-track in the FP
									if(SubTrack_it != EssStrInf[iTrack].FPSubTracks.end())
									{
										ThisDesc->SetUInt(LinkedTrackID_UL, (*(SubTrack_it++))->GetUInt(TrackID_UL));
									}
								}

								// Add all essence container ULs, we read these from the sub-descriptors as WrapOpt only has space for one
								MDObject *ECLabel = ThisDesc->Child(EssenceContainer_UL);
								if(ECLabel)
								{
									// If this is a dictionary ref, pull out the identification from the dictionary
									if(ECLabel->GetRef()) ECLabel = ECLabel->GetRef()->Child(DefinitionObjectIdentification_UL);

									if(ECLabel)
									{
										DataChunkPtr LabelData = ECLabel->PutData();
										if(LabelData && (LabelData->Size == 16))
										{
											UL ThisLabel(LabelData->Data);
											MData->AddEssenceType(ThisLabel);
										}
									}
								}
							}
							it++;
						}
						MData->AddEssenceType((*WrapCfgList_it)->WrapOpt->WrappingUL);
					}
				}
				else
				{
					// Write a SubDescriptor
					(*WrapCfgList_it)->EssenceDescriptor->SetUInt(LinkedTrackID_UL, EssStrInf[iTrack].FPTrack->GetUInt(TrackID_UL));

					MuxDescriptor[FileDescriptors_UL]->AddChild()->MakeLink((*WrapCfgList_it)->EssenceDescriptor);

					MData->AddEssenceType((*WrapCfgList_it)->WrapOpt->WrappingUL);
				}

				// Link the MP to the FP
				if(EssStrInf[iTrack].MPClip) EssStrInf[iTrack].MPClip->MakeLink(EssStrInf[iTrack].FPTrack, 0);

				// Link sub-MP clips to sub-FP clips
				TrackList::iterator m_it = EssStrInf[iTrack].MPSubTracks.begin();
				TrackList::iterator f_it = EssStrInf[iTrack].FPSubTracks.begin();
				while((m_it != EssStrInf[iTrack].MPSubTracks.end()) && (f_it != EssStrInf[iTrack].FPSubTracks.end()))
				{
					(*m_it)->AddSourceClip()->MakeLink((*f_it), 0);
					m_it++;
					f_it++;
				}
			}
		}
		else // !pOpt->OPAtom, !FrameGroup
		{
			if(WriteFP)
			{
				// Sort out TrackIDs if the new descriptor is a multiple descriptor
				if((*WrapCfgList_it)->EssenceDescriptor->IsA(MultipleDescriptor_UL))
				{
					MDObjectPtr SubDesc = (*WrapCfgList_it)->EssenceDescriptor->Child(FileDescriptors_UL);
					if(SubDesc)
					{
						TrackList::iterator SubTrack_it = EssStrInf[iTrack].FPSubTracks.begin();
						MDObject::iterator it = SubDesc->begin();
						while(it != SubDesc->end())
						{
							MDObjectPtr ThisDesc = (*it).second->GetRef();
							if(ThisDesc)
							{
								// Work out the TrackID for each sub-descriptor
								if(it == SubDesc->begin())
								{
									// The first stream will have ended up ad the master FPTrack
									ThisDesc->SetUInt(LinkedTrackID_UL, EssStrInf[iTrack].FPTrack->GetUInt(TrackID_UL));
								}
								else
								{
									// Each further stream will be a sub-track in the FP
									if(SubTrack_it != EssStrInf[iTrack].FPSubTracks.end())
									{
										ThisDesc->SetUInt(LinkedTrackID_UL, (*(SubTrack_it++))->GetUInt(TrackID_UL));
									}
								}

								// Add all essence container ULs, we read these from the sub-descriptors as WrapOpt only has space for one
								MDObject *ECLabel = ThisDesc->Child(EssenceContainer_UL);
								if(ECLabel)
								{
									// If this is a dictionary ref, pull out the identification from the dictionary
									if(ECLabel->GetRef()) ECLabel = ECLabel->GetRef()->Child(DefinitionObjectIdentification_UL);

									if(ECLabel)
									{
										DataChunkPtr LabelData = ECLabel->PutData();
										if(LabelData && (LabelData->Size == 16))
										{
											UL ThisLabel(LabelData->Data);
											MData->AddEssenceType(ThisLabel);
										}
									}
								}
							}
							it++;
						}
					}
				}

				// Add the essence type
				MData->AddEssenceType((*WrapCfgList_it)->WrapOpt->WrappingUL);

				// Check that we are not about to add a second descriptor to this file package (e.g. because we are processing a sub-stream such as captions)
				MDObjectPtr Descriptor = FilePackage->GetRef(Descriptor_UL);
				if(!Descriptor)
				{
					// Write a FileDescriptor
					// DRAGONS Can we ever need a MultipleDescriptor?
					FilePackage->AddChild(Descriptor_UL)->MakeLink((*WrapCfgList_it)->EssenceDescriptor);

					if((*WrapCfgList_it)->EssenceDescriptor->IsA(MultipleDescriptor_UL))
					{
						// Ensure that we have flagged a multiple descriptor if one is used
						ULPtr GCUL = new UL( mxflib::GCMulti_Data );
						MData->AddEssenceType( GCUL );
					}
					else
						// FIXME: Need to find a way to sort track IDs for multiples at some point
						(*WrapCfgList_it)->EssenceDescriptor->SetUInt(LinkedTrackID_UL, EssStrInf[iTrack].FPTrack->GetUInt(TrackID_UL));

				}
				else
				{
					/* Already added a descriptor to this file package */

					// If the existing descriptor is not a file descriptor, we need to convert it
					if(!Descriptor->IsA(MultipleDescriptor_UL))
					{
						// Build a new multiple descriptor
						MDObjectPtr MuxDescriptor = new MDObject(MultipleDescriptor_UL);

						MDObjectPtr SampleRate = Descriptor->Child(SampleRate_UL);
						if(SampleRate)
						{
							MuxDescriptor->AddChild(SampleRate_UL)->SetInt("Numerator", SampleRate->GetInt("Numerator"));
							MuxDescriptor->AddChild(SampleRate_UL)->SetInt("Denominator", SampleRate->GetInt("Denominator"));
						}

						MuxDescriptor->AddChild(EssenceContainer_UL,false)->SetValue(DataChunk(16,mxflib::GCMulti_Data));

						// Ensure that we have flagged a multiple descriptor
						ULPtr GCUL = new UL( mxflib::GCMulti_Data );
						MData->AddEssenceType( GCUL );

						// Add the existing descriptor as the first sub-descriptor
						MuxDescriptor->AddRef(FileDescriptors_UL, Descriptor);

						// Make the multi descriptor the new file descriptor
						FilePackage->MakeRef(Descriptor_UL, MuxDescriptor);
						Descriptor = MuxDescriptor;
					}

					// The new descriptor is a multiple descriptor already
					if((*WrapCfgList_it)->EssenceDescriptor->IsA(MultipleDescriptor_UL))
					{
						// Add each child of the new multiple descriptor to the old one
						MDObjectPtr FileDescriptors = (*WrapCfgList_it)->EssenceDescriptor->Child(FileDescriptors_UL);
						if(FileDescriptors)
						{
							MDObject::iterator it = FileDescriptors->begin();
							while(it != FileDescriptors->end())
							{
								MDObjectPtr Ptr = (*it).second->GetRef();
								if(Ptr) Descriptor->AddRef(FileDescriptors_UL, Ptr);
								it++;
							}
						}
					}
					else
					{
						// Add the new descriptor as a sub-descriptor
						Descriptor->AddRef(FileDescriptors_UL, (*WrapCfgList_it)->EssenceDescriptor);
					}
				}

				// Link the MP to the FP
				if(EssStrInf[iTrack].MPClip) EssStrInf[iTrack].MPClip->MakeLink(EssStrInf[iTrack].FPTrack, 0);

				// Link sub-MP clips to sub-FP clips
				TrackList::iterator m_it = EssStrInf[iTrack].MPSubTracks.begin();
				TrackList::iterator f_it = EssStrInf[iTrack].FPSubTracks.begin();
				while((m_it != EssStrInf[iTrack].MPSubTracks.end()) && (f_it != EssStrInf[iTrack].FPSubTracks.end()))
				{
					(*m_it)->AddSourceClip()->MakeLink((*f_it), 0);
					m_it++;
					f_it++;
				}
			}
		}

		// Record the file package index used this time
		PreviousFP = Source[iTrack].first;

		WrapCfgList_it++;
		iTrack++;
	}

	/* Ensure that building frame-wrap groups has not added a multiple descriptor containing a single descriptor in any File Package */
	if(pOpt->FrameGroup)
	{
		PackageList::iterator it = MData->Packages.begin();
		while(it != MData->Packages.end())
		{
			if((*it)->IsA(SourcePackage_UL))
			{
				MDObjectPtr Descriptor = (*it)->GetRef(Descriptor_UL);
				if(Descriptor && Descriptor->IsA(MultipleDescriptor_UL))
				{
					MDObjectPtr Descriptors = Descriptor[FileDescriptors_UL];

					if((!Descriptors) || Descriptors->empty())
					{
						error("Ended up with an empty MultipleDescriptor - are we building an empty file?\n");
					}
					else if(Descriptors->size() == 1)
					{
						// Link the single contained descriptor directly from the file package so the multiple descriptor will evaporate now it is unreferenced
						MDObjectPtr Link = Descriptors->front().second->GetRef();
						if(Link) FilePackage->MakeRef(Descriptor_UL, Link);
					}
					else
					{
						// Ensure that we have flagged a multiple descriptor
						// DRAGONS: We do this here so that it is only added when genuinely required
						ULPtr GCUL = new UL( mxflib::GCMulti_Data );
						MData->AddEssenceType( GCUL );
					}
				}
			}
			it++;
		}
	}


}
