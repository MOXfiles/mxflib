/*! \file	process_utils.h
 *	\brief	MXF wrapping utilities
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
