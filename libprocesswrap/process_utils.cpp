/*! \file	process_utils.cpp
 *	\brief	MXF wrapping functions
 *
 *	\version $Id$
 *
 */
/*
 *	Copyright (c) 2010, Metaglue Corporation
 */

#include "process_utils.h"

#include <stdio.h>
#include <iostream>
#include <string>

using namespace std;

#include "mxflib/mxflib.h"
using namespace mxflib;

#include "libprocesswrap/process.h"

//! Convert a frame-count timecode from one edit rate to another
/*! \param Timecode The timecode in CurrentEditRate
*  \param CurrentEditRate The edit rate of the input timecode
*  \param DesiredEditRate The desired output edit rate
*  \param AllowErrors If set to false error messages will be suppressed
*  \return The frame-count edit rate, or 0 if any error occured
*  \note Error messages will be produced if you attempt to convert a non-zero timecode to or from a zero edit rate, or beyond the range of 64-bit maths.
*  \note No error message will be produced if there is a zero input Timecode, even if other parameters are bad
*/
Position ConvertTimecode(Position Timecode, Rational CurrentEditRate, Rational DesiredEditRate, bool AllowErrors /* = true */)
{
	// No need to adjust if timecode starts at zero
	if(Timecode != 0)
	{
		// Must check the data before correcting to avoid divide by 0!
		if(CurrentEditRate.Numerator == 0)
		{
			if(AllowErrors) error("Can't convert timecode to a zero edit rate, setting to zero\n");
			Timecode = 0;
		}
		else if(DesiredEditRate.Denominator == 0)
		{
			if(AllowErrors) error("Can't convert timecode from zero edit rate, setting to zero\n");
			Timecode = 0;
		}
		else
		{
			// Ensure we are working on a level ground
			CurrentEditRate.Reduce();
			DesiredEditRate.Reduce();

			// Should we correct?
			if(!(CurrentEditRate == DesiredEditRate))
			{
				// Is it safe to do the adjustment
				// TODO: This range check is over-cautious, it could be better
				Int64 TCMultiply = CurrentEditRate.Denominator;
				TCMultiply *= DesiredEditRate.Numerator;
				if((Timecode > INT64_C(0x00000000ffffffff)) || (TCMultiply > INT64_C(0x00000000ffffffff)))
				{
					if(AllowErrors) error("Unable to frame-rate correct timecode due to huge timecode or extreme edit-rate, setting to zero\n");
					Timecode = 0;
				}
				else
				{
					Timecode *= TCMultiply;
					Timecode /= CurrentEditRate.Numerator * DesiredEditRate.Denominator;
				}
			}
		}
	}

	// Return the adjusted timecode
	return Timecode;
}


//! Short term hack to allow per-BodySID GCWriters
/*! DRAGONS: This needs to be tidied a little when there is time! */
GCWriterPtr AddGCWriter(std::map<int, GCWriterPtr> &Map, MXFFilePtr &File, int BodySID)
{
	// First try and return an existing GCWriter
	std::map<int, GCWriterPtr>::iterator it = Map.find(BodySID);
	if(it != Map.end()) return (*it).second;

	// Insert a new writer
	Map.insert(std::map<int, GCWriterPtr>::value_type(BodySID, new GCWriter(File, BodySID)));

	// Find and return the new entry (not hugely efficient!)
	it = Map.find(BodySID);

	return (*it).second;
}


//! Set the index options for a given body stream
void SetStreamIndex(BodyStreamPtr &ThisStream, bool isCBR, ProcessOptions		*pOpt)
{
	// Set CBR indexing flags
	if(isCBR)
	{
		// If this stream is not CBR indexable don't set any flags
		if(ThisStream->GetSource()->GetBytesPerEditUnit() == 0) return;

		if(pOpt->IsolatedIndex)
		{
			if(pOpt->UseIndex||pOpt->SprinkledIndex) ThisStream->SetIndexType( (BodyStream::IndexType) ( BodyStream::StreamIndexCBRHeaderIsolated 
				| BodyStream::StreamIndexCBRFooter) );
		}
		else
		{
			if(pOpt->UseIndex||pOpt->SprinkledIndex) ThisStream->SetIndexType( (BodyStream::IndexType) ( BodyStream::StreamIndexCBRHeader 
				| BodyStream::StreamIndexCBRBody 
				| BodyStream::StreamIndexCBRFooter) );
		}
	}
	// Set VBR indexing flags
	else
	{
		// If this stream is not VBR indexable don't set any flags
		if(!ThisStream->GetSource()->CanIndex()) return;

		if(pOpt->UseIndex) 
			ThisStream->AddIndexType(BodyStream::StreamIndexFullFooter);
		if(pOpt->SparseIndex) 
			ThisStream->AddIndexType(BodyStream::StreamIndexSparseFooter);
		if(pOpt->SprinkledIndex) 
		{
			if(pOpt->IsolatedIndex) 
				ThisStream->AddIndexType(BodyStream::StreamIndexSprinkledIsolated);
			else 
				ThisStream->AddIndexType(BodyStream::StreamIndexSprinkled);
		}

	}
}


//! Set the wrapping type for a stream
void SetStreamWrapType(BodyStreamPtr &ThisStream, WrappingOption::WrapType Type)
{
	if(Type == WrappingOption::Frame) ThisStream->SetWrapType(BodyStream::StreamWrapFrame);
	else if(Type == WrappingOption::Clip) ThisStream->SetWrapType(BodyStream::StreamWrapClip);
	else ThisStream->SetWrapType(BodyStream::StreamWrapOther);
}

