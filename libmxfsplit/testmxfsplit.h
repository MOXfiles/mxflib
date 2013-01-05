/*! \file	testmxfsplit.h
 *	\brief	test for unwrap library for MXFLib
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
#ifndef _TESTMXFSPLIT_H_
#define _TESTMXFSPLIT_H_

//! Simple test of Processor-based splitting
/*! Demonstrates the entire process of splitting a region in a file
 *	\param	File path and name of MXF file to be split
 *	\param	FirstFrame Position of start of the region to be split, measured in EditUnits
 *	\param	nFrames Length of the region to be split, measured in EditUnits
 *	\param	ProcSink char to choose which kind of EssenceSink ('d'=DataChunkSink, 'e'=ExternalBufferSink
 *	\return	0 if all successful
 */
int Simple( const char *File, const unsigned FirstFrame, const unsigned nFrames, const char ProcSink ='\0' );

#endif // _TESTMXFSPLIT_H_
