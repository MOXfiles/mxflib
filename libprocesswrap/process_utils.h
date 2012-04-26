/*! \file	process_utils.h
 *	\brief	MXF wrapping utilities
 *
 *	\version $Id$
 *
 */
/*
 *	Copyright (c) 2011, Metaglue Corporation. All Rights Reserved.
 */

#ifndef _process_utils_h_
#define _process_utils_h_

#include "mxflib/mxflib.h"
using namespace mxflib;

#include "libprocesswrap/process.h"


// FIXME to be moved to mxflib/types.hcpp
Position ConvertTimecode(Position Timecode, Rational CurrentEditRate, Rational DesiredEditRate, bool AllowErrors = true);

// FIXME unused "short term hack"
GCWriterPtr AddGCWriter(std::map<int, GCWriterPtr> &Map, MXFFilePtr &File, int BodySID);

//! Set the index options for a given body stream
void SetStreamIndex(BodyStreamPtr &ThisStream, bool isCBR, ProcessOptions		*pOpt);

//! Set the wrapping type for a stream
void SetStreamWrapType(BodyStreamPtr &ThisStream, WrappingOption::WrapType Type);

#endif // _process_utils_h_
