/*! \file	process.cpp
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

#include <stdio.h>
#include <iostream>
#include <string>
using namespace std;

#include "mxflib/mxflib.h"
using namespace mxflib;


#include "process.h"
#include "process_utils.h"


#include "productIDs.h"


FILE * hLogout=stdout; //file handle to send all the informational output to

std::string GetVersionText()
{
	char VersionText[256];
	snprintf(VersionText, 255, "MXFWrap %s.%s.%s(%s)%s of %s %s", PRODUCT_VERSION_MAJOR, PRODUCT_VERSION_MINOR, PRODUCT_VERSION_TWEAK, PRODUCT_VERSION_BUILD,
		     MXFLIB_VERSION_RELTEXT(PRODUCT_VERSION_REL), __DATE__, __TIME__);
	return VersionText;
}



void SetUpIndex(int OutFileNum,
				ProcessOptions    *pOpt,
				MetadataPtr       MData,
				EssenceSourcePair *Source,
				EssenceParser::WrappingConfigList WrapCfgList,
				EssenceStreamInfo*EssStrInf)
{
	EssenceParser::WrappingConfigList::iterator WrapCfgList_it;
	// Find all essence container data sets so we can update "IndexSID"
	MDObjectPtr ECDataSets = MData[ContentStorageObject_UL];
	if(ECDataSets) ECDataSets = ECDataSets->GetLink();
	if(ECDataSets) ECDataSets = ECDataSets[EssenceDataObjects_UL];

	int PreviousFP = -1;								// The index of the previous file package used - allows us to know if we treat this is a sub-stream
	int iStream = -1;									// Stream index (note that it will be incremented to 0 in the first iteration)
	int iTrack=0;
	bool IndexAdded = false;							// Set once we have added an index so we only add to the first stream in frame-group
	WrapCfgList_it = WrapCfgList.begin();
	while(WrapCfgList_it != WrapCfgList.end())
	{

		// Move on to a new stream if we are starting a new file package
		if(Source[iTrack].first != PreviousFP) iStream++;

		// Only process the index for the first stream of a file package
		if((Source[iTrack].first != PreviousFP || pOpt->OPAtom ) && (!(*WrapCfgList_it)->IsExternal))
		{
		// Write File Packages except for externally ref'ed essence in OP-Atom
			bool WriteFP = (!pOpt->OPAtom) || (iStream == OutFileNum);

			if(WriteFP)
			{
				// Only index it if we can
				// Currently we can only VBR index frame wrapped essence
				// FIXME: We enable the VBR mode twice doing it this way, which is not ideal - should we cache the result? Or do we even need to check?
				if(    ((*WrapCfgList_it)->WrapOpt->CBRIndex && (Source[iTrack].second->GetBytesPerEditUnit() != 0))
					|| ((*WrapCfgList_it)->WrapOpt->CanIndex 
					|| ((*WrapCfgList_it)->WrapOpt->ThisWrapType == WrappingOption::Frame
					|| Source[iTrack].second->EnableVBRIndexMode() )))
				{
					if(		( pOpt->OPAtom && iTrack==OutFileNum)
						||	(!pOpt->OPAtom && pOpt->FrameGroup && (!IndexAdded))
						||	(!pOpt->OPAtom && !pOpt->FrameGroup) )
					{
						UInt32 BodySID;				// Body SID for this essence stream
						UInt32 IndexSID;			// Index SID for the index of this essence stream

						IndexAdded = true;
							BodySID = EssStrInf[iStream].Stream->GetBodySID();
							IndexSID = BodySID + 128;
							EssStrInf[iStream].Stream->SetIndexSID(IndexSID);

						// Update IndexSID in essence container data set
						if(ECDataSets)
						{
							MDObject::iterator ECD_it = ECDataSets->begin();
							while(ECD_it != ECDataSets->end())
							{
								if((*ECD_it).second->GetLink())
								{
									if((*ECD_it).second->GetLink()->GetUInt(BodySID_UL) == BodySID)
									{
										(*ECD_it).second->GetLink()->SetUInt(IndexSID_UL, IndexSID);
										break;
									}
								}
								ECD_it++;
							}
						}
					}
				}
			}
		}

		// Record the file package index used this time
		PreviousFP = Source[iTrack].first;

		WrapCfgList_it++;
		iTrack++;
	}
}
//!return Essence duration
Length ProcessEssence(int OutFileNum,
					  ProcessOptions    *pOpt,
					  EssenceSourcePair *Source,
					  EssenceParser::WrappingConfigList WrapCfgList,
					  BodyWriterPtr      Writer,
					  Rational			  EditRate,
					  MetadataPtr        MData,
					  EssenceStreamInfo *EssStrInf,
					  TimecodeComponentPtr MPTimecodeComponent
					  )
{
#ifdef _WIN32
	LARGE_INTEGER  start;
	QueryPerformanceCounter(&start);
#else
	struct timeval start;
	struct timezone tz;
	gettimeofday(& start, &tz);
#endif

	// Write the body
	if(pOpt->BodyMode == Body_None)
	{
		Writer->WriteBody();
	}
	else
	{
		while(!Writer->BodyDone())
		{
			if(pOpt->BodyMode == Body_Duration)
				Writer->WritePartition(pOpt->BodyRate, 0);
			else
				Writer->WritePartition(0, pOpt->BodyRate);
		}
	}

	// Work out the durations
	Length EssenceDuration;

	Int32 IndexBaseTrack;
	if( pOpt->OPAtom ) IndexBaseTrack = OutFileNum;
	else if( pOpt->FrameGroup ) IndexBaseTrack = 0;
	else  IndexBaseTrack = 0;

	if(EssStrInf[IndexBaseTrack].Stream)
		EssenceDuration = (Length) EssStrInf[IndexBaseTrack].Stream->GetSource()->GetCurrentPosition();
	else
		EssenceDuration = -1;

#ifdef DEMO

	if(EssenceDuration>20)
	{
		puts("Evaluation version cannot make file this long");
		return 0;
	}
#endif


#ifdef _WIN32
	LARGE_INTEGER  end;
	QueryPerformanceCounter(&end);
	LARGE_INTEGER Freq;
	QueryPerformanceFrequency(&Freq);
	if(Freq.QuadPart!=0)
	{
		__int64 diff=end.QuadPart-start.QuadPart;
		float time=((float)diff)/Freq.QuadPart;
		float fps=EssenceDuration/time;


		if(pOpt->ShowTiming) printf("Completed %s samples at %4.3f per second\n", Int64toString(EssenceDuration).c_str(), fps);
		else printf("Completed %s samples\n", Int64toString(EssenceDuration).c_str());

	}
#else
	struct timeval end;
	gettimeofday(& end, &tz);
	time_t secs=end.tv_sec-start.tv_sec;
	int usecs=end.tv_usec-start.tv_usec;
	float time=(float)secs+(float)usecs/1000000.0;
	float fps=EssenceDuration/time;

	if(pOpt->ShowTiming) printf("Completed %s samples at %4.3f per second\n", Int64toString(EssenceDuration).c_str(), fps);
	else printf("Completed %s samples\n", Int64toString(EssenceDuration).c_str());

#endif

	// Update the modification time
	MData->SetTime();

	// Update all durations
	// (Index Duration forced above)

	// Update Material Package Timecode Track Duration
	Length EditRateDuration = (Length) EssenceDuration * ( EditRate/(EssStrInf[IndexBaseTrack].Stream->GetSource()->GetEditRate()) );
	fprintf( hLogout,"EditRateDuration = %s\n", Int64toString(EditRateDuration).c_str());

	if(MPTimecodeComponent) MPTimecodeComponent->SetDuration(EditRateDuration);

	EssenceParser::WrappingConfigList::iterator WrapCfgList_it;

	int PreviousFP = -1;								// The index of the previous file package used - allows us to know if we treat this is a sub-stream
	int iStream = -1;									// Stream index (note that it will be incremented to 0 in the first iteration)
	int iTrack = 0;
	WrapCfgList_it = WrapCfgList.begin();
	while(WrapCfgList_it != WrapCfgList.end())
	{
		// Move on to a new stream if we are starting a new file package
		if(Source[iTrack].first != PreviousFP) iStream++;


		if(EssStrInf[iTrack].MPClip)
		{
			EssStrInf[iTrack].MPClip->SetDuration(EditRateDuration);

			// Set sub-track durations
			if(!EssStrInf[iTrack].MPSubTracks.empty())
			{
				TrackList::iterator m_it = EssStrInf[iTrack].MPSubTracks.begin();
				while(m_it != EssStrInf[iTrack].MPSubTracks.end())
				{
					if(!(*m_it)->Components.empty()) (*m_it)->Components.front()->SetDuration(EditRateDuration);
					m_it++;
				}
			}

			if( (!pOpt->OPAtom) || (iStream == OutFileNum) )
			{
				if(EssStrInf[iTrack].FPTimecodeComponent) EssStrInf[iTrack].FPTimecodeComponent->SetDuration(EditRateDuration);

					EssStrInf[iTrack].FPClip->SetDuration(EssenceDuration);

				//IDB july2012 this line was here negating the point of the logic above 
				//           - I assume there is no reason so we can delete in a bit if there are no consequences
				//EssStrInf[iTrack].FPClip->SetDuration(EssenceDuration);

				// Set sub-track durations
				if(!EssStrInf[iTrack].FPSubTracks.empty())
				{
					TrackList::iterator f_it = EssStrInf[iTrack].FPSubTracks.begin();
					while(f_it != EssStrInf[iTrack].FPSubTracks.end())
					{
						if(!(*f_it)->Components.empty()) (*f_it)->Components.front()->SetDuration(EssenceDuration);
						f_it++;
					}
				}

				// Set file descriptor durations
				(*WrapCfgList_it)->EssenceDescriptor->SetInt64(ContainerDuration_UL,EssenceDuration);
				if((*WrapCfgList_it)->EssenceDescriptor->IsA(MultipleDescriptor_UL))
				{
					MDObjectPtr FileDescriptors = (*WrapCfgList_it)->EssenceDescriptor->Child(FileDescriptors_UL);
					if(FileDescriptors)
					{
						MDObject::iterator it = FileDescriptors->begin();
						while(it != FileDescriptors->end())
						{
							if((*it).second->GetRef()) ((*it).second->GetRef())->SetInt64(ContainerDuration_UL,EssenceDuration);
							it++;
						}
					}
				}

				// If frame grouping, we will have added a manufactured Multiple Descriptor, so set its duration
				// DRAGONS: This will get called for each time around the loop, so we set the duration multiple times, but this is not an issue
				if(pOpt->FrameGroup)
				{
					if(EssStrInf[iTrack].FPTrack->GetParent())
					{
						MDObjectPtr Descriptor = EssStrInf[iTrack].FPTrack->GetParent()->GetRef(Descriptor_UL);
						if(Descriptor) Descriptor->SetInt64(ContainerDuration_UL,EssenceDuration);
					}
				}

				// Update origin if required
				// DRAGONS: This is set in the File Package - the spec seems unclear about which Origin should be set!
				Position Origin = Source[iTrack].second->GetPrechargeSize();
				if(Origin)
				{
					TrackParent FPTrack = EssStrInf[iTrack].FPClip->GetParent();
					if(FPTrack)	FPTrack->SetInt64(Origin_UL, Origin);
				}
			}

		}

		// Record the file package index used this time
		PreviousFP = Source[iTrack].first;

		WrapCfgList_it++;
		iTrack++;
	}


	// return the finished length to the caller
	return EssenceDuration;
}

#include "process_metadata.h"


//! Process an output file
Length Process(
			   int					OutFileNum,
			   MXFFilePtr			Out,
			   ProcessOptions		*pOpt,
			   EssenceParser::WrappingConfigList WrapCfgList,
			   EssenceSourcePair	*Source,
			   Rational			EditRate,
			   UMIDPtr				MPUMID,
			   UMIDPtr				*FPUMID,
			   UMIDPtr				*SPUMID,
			   bool				*pReadyForEssenceFlag  /* =NULL */
			   )

{
	TimecodeComponentPtr MPTimecodeComponent ;

	Length Ret = 0;

	EssenceStreamInfo EssStrInf[ProcessOptions::MaxInFiles];
	// FP UMIDs are the same for all OutFiles, so they are supplied as a parameter


	PackagePtr FilePackage;


	/* Step: Create a set of header metadata */

	MetadataPtr MData = new Metadata();
	mxflib_assert(MData);
	mxflib_assert(MData->Object);

	// Build the body writer
	BodyWriterPtr Writer = new BodyWriter(Out);


#if defined FORCEGCMULTI 
	// 377M MultipleDescriptor (D.5) requires an EssenceContainer label (D.1), which must be this
	// degenerate label (see mxfIG FAQ). Therefore the degenerate value must also appear in the
	// Header (A.1) and partition pack...
	// also, explicitly required by AS-CNN sec 2.1.6

	// DRAGONS: Why is this here? It unconditionally adds "Used to describe multiple wrappings not
	//          otherwise covered under the MXF Generic Container node" to all MXF files!!


	// Assume we are doing GC
	ULPtr GCUL = new UL( mxflib::GCMulti_Data );
	MData->AddEssenceType( GCUL );

	// This appears to be acceptable to Avid XpressProHD 5.1.2
#endif



	// Process Metadata

		ProcessMetadata( OutFileNum, pOpt,
			Source, WrapCfgList, EditRate, Writer, MData, MPUMID,
			FPUMID, SPUMID, EssStrInf, FilePackage, 
			MPTimecodeComponent  //OUT variables
			);

	//
	// ** Set up IndexSID **
	//
	if(pOpt->UseIndex || pOpt->SparseIndex || pOpt->SprinkledIndex)
	{
		SetUpIndex(OutFileNum,pOpt, MData,Source,WrapCfgList,EssStrInf);
	}

	//
	// ** Set up the base partition pack **
	//

	PartitionPtr ThisPartition = new Partition(OpenHeader_UL);
	mxflib_assert(ThisPartition);
	ThisPartition->SetKAG(pOpt->KAGSize);			// Everything else can stay at default
	ThisPartition->SetUInt(BodySID_UL, 1);

	// Build an Ident set describing us and link into the metadata
	MDObjectPtr Ident=Metadata::MakeIdent( CompanyName,
										   Product_UUID,
										   ProductName,
										   ProductVersionString,
										   ProductProductVersion);

	// Link the new Ident set with all new metadata
	// Note that this is done even for OP-Atom as the 'dummy' header written first
	// could have been read by another device. This flags that items have changed.
	MData->UpdateGenerations(Ident);

	ThisPartition->AddMetadata(MData);

	// Add the template partition to the body writer
	Writer->SetPartition(ThisPartition);

	//
	// ** Process Essence **
	//

	// Write the header (open and incomplete so far)

	// Set block alignment for Avid compatibility
	// with an extra -ve offset for essence to align the V rather than the K

	const UInt32 PartitionPackLength = 0x7c;

	const UInt32 AvidBlockSize = 0x60000;
	const UInt32 AvidKAGSize = 512;
	const UInt32 AvidIndexBERSize = 9;

	const UInt32 ULSize = 16;
	int DynamicOffset = 0-ULSize;


	// Kludge to find the most likely BERSize
	EssenceSourcePtr Stream0 = EssStrInf[ OutFileNum ].Stream ? *(EssStrInf[ OutFileNum ].Stream->begin()) : EssenceSourcePtr(NULL);
	if( !Stream0 || Stream0->GetBERSize() == 0) 
	{
		if(Stream0 && (EssStrInf[ OutFileNum ].Stream->GetWrapType() == ClipWrap) ) DynamicOffset -= 8;
		else DynamicOffset -= 4;
	}
	else DynamicOffset -= Stream0->GetBERSize();

		if( pOpt->BlockSize )
		{
			// set dynamic default if -ko=-1000
			if( pOpt->BlockOffset == -1000 ) pOpt->BlockOffset = DynamicOffset;

			Out->SetBlockAlign( pOpt->BlockSize, pOpt->BlockOffset, pOpt->BlockIndexOffset );
		}

		// Use padding per command line - even for block aligned files
		if(pOpt->HeaderPadding) Writer->SetPartitionFiller(pOpt->HeaderPadding);
		if(pOpt->HeaderSize) Writer->SetPartitionSize(pOpt->HeaderSize);

		// DRAGONS: would be nice to have an even length Header Partition
		//if(pOpt->HeaderSize) Writer->SetPartitionSize(pOpt->HeaderSize - PartitionPackLength);

		Writer->WriteHeader(false, false);


		// If we are writing OP-Atom update the OP label so that body partition packs claim to be OP-Atom
		// The header will remain as a generalized OP until it is re-written after the footer
		if( pOpt->OPAtom ) 
		{
			MData->SetOP(OPAtomUL);
		}

		if( pOpt->OPAtom )
		{
			// Set top-level file package correctly for OP-Atom
			// DRAGONS: This will need to be changed if we ever write more than one File Package for OP-Atom!
			if( FilePackage) MData->SetPrimaryPackage(FilePackage);
		}


		if(pReadyForEssenceFlag)
			*pReadyForEssenceFlag=true;

		Ret=ProcessEssence(OutFileNum,pOpt,Source,WrapCfgList,
			Writer,EditRate, MData,EssStrInf,MPTimecodeComponent
			);

		// Update SourcePackage Timecode Duration
		// DRAGONS: since we are assuming a 24 hour Source, don't need this
		// if( SPTimecodeComponent ) SPTimecodeComponent->SetDuration(EssenceDuration);

		// Update SourcePackage Edgecode Duration
		// DRAGONS: since we are assuming a 10000 foot Source, don't need this
		// if( SPEdgecodeComponent ) SPEdgecodeComponent->SetDuration(EssenceDuration);

		// Update the generation UIDs in the metadata to reflect the changes
		MData->UpdateGenerations(Ident);

		// Make sure any new sets are linked in
		ThisPartition->UpdateMetadata(MData);

		// Actually write the footer
		// Note: No metadata in OP-Atom footer
		if(pOpt->OPAtom) Writer->WriteFooter(false);
		else Writer->WriteFooter(true, true);

		//
		// ** Update the header ** 
		//
		// For generalized OPs update the value of "FooterPartition" in the header pack
		// For OP-Atom re-write the entire header
		//

		UInt64 FooterPos = ThisPartition->GetUInt64(FooterPartition_UL);
		Out->Seek(0);

		DataChunkPtr IndexData;

		if(pOpt->UpdateHeader)
		{
#ifndef WIN32
			static pthread_mutex_t mutex=PTHREAD_MUTEX_INITIALIZER;
			pthread_mutex_lock(&mutex);
#endif

			// Read the old partition to allow us to keep the same KAG and SIDs
			PartitionPtr OldHeader = Out->ReadPartition();

			// Read any index table data
			IndexData = OldHeader->ReadIndexChunk();

			// If the header did not contain any index data, see if we usefully can add some - we will ditch it if the update fails
			bool AddingIndex = false;
			if(!IndexData)
			{
				// Search for the appropriate index table to add
				BodyStream *pStream = NULL;
				IndexManager *pManager = NULL;

				// If the header has essence - it must be index from that essence
				UInt32 BodySID = OldHeader->GetUInt(BodySID_UL);

				if(BodySID)
				{
					// Get the manager if the header-essence is indexed
					pStream = Writer->GetStream(BodySID);
					if(pStream) pManager = pStream->GetIndexManager();
				}
				else
				{
					// Scan all known streams...
					for(;;)
					{
						BodySID = Writer->GetNextUsedBodySID(BodySID);
						if(BodySID == 0) break;
						
						/// ... looking for a CBR index
						pStream = Writer->GetStream(BodySID);
						if(pStream) pManager = pStream->GetIndexManager();
						if(pManager && pManager->IsCBR()) break;

						// TODO: We could try VBR if we know it will fit!

						if(pManager) break;
					}
				}

				// So here we either have the manager for the essence in the header,
				// or the manager for the first CBR essence stream,
				// or NULL

				// Read the index types and see what is requested
				BodyStream::IndexType IndexFlags = pStream->GetIndexType();

				if(pManager && ( IndexFlags&(BodyStream::StreamIndexSparseFooter) ) )
				{


					IndexTablePtr Index = pManager->MakeIndex();
					if(Index)
					{
						if(!pManager->IsCBR()) pManager->AddEntriesToIndex(Index);
						IndexData = new DataChunk();
						
						Index->WriteIndex(*IndexData);
						ThisPartition->SetUInt(IndexSID_UL, pManager->GetIndexSID());

						AddingIndex = true;
					}
				}
			}

			// Now update the partition we are about to write (the one with the metadata)
			ThisPartition->ChangeType(ClosedCompleteHeader_UL);
			ThisPartition->SetUInt64(FooterPartition_UL, FooterPos);
			ThisPartition->SetKAG(OldHeader->GetUInt(KAGSize_UL));
			
			// DRAGONS: We don't copy over the IndexSID if the code to add a new index table has added a new one in
			if(!AddingIndex) ThisPartition->SetUInt(IndexSID_UL, OldHeader->GetUInt(IndexSID_UL));
			ThisPartition->SetUInt64(BodySID_UL, OldHeader->GetUInt(BodySID_UL));

			Out->Seek(0);
			if(IndexData)
			{
				// Try and re-write with the index table, if this will not fit (and we have added the index in this update) remove it and try again
				bool Result = Out->ReWritePartitionWithIndex(ThisPartition, IndexData);
				if(AddingIndex && (!Result))
				{
					ThisPartition->SetUInt(IndexSID_UL, 0);
					Out->ReWritePartition(ThisPartition);
					fprintf( hLogout,"Note: An attempt was made to add a full index table to the Header.\n");
					fprintf( hLogout,"      This failed, but the header is still valid without the index table.\n");
				}
			}
			else
				Out->ReWritePartition(ThisPartition);
#ifndef WIN32
			pthread_mutex_unlock(&mutex);
#endif
		}
		else
		{
			ThisPartition = Out->ReadPartition();
			ThisPartition->SetUInt64(FooterPartition_UL, FooterPos);
			Out->Seek(0);
			Out->WritePartitionPack(ThisPartition);
		}


		return Ret;
}
