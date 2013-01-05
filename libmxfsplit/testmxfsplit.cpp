/*! \file	testmxfsplit.cpp
 *	\brief	unwrap library for MXFLib
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

#include "testmxfsplit.h"

#include "mxflib/mxflib.h"
using namespace mxflib;

#include "libmxfsplit.h"
#include "containerinfo.h"

//! very basic usage assumes all Descriptors wanted
bool Wanted( DescriptorPtr d ){ return true; }

//! Simple test of Processor-based splitting
int Simple( const char *file, const unsigned firstFrame, const unsigned nFrames, const char ProcSink /*='\0'*/ )
{
	int Ret = 0;
	switch( ProcSink )
	{
	case 'd': printf("Processor-based splitting with DataChunkSink\n"); break;
	case 'e': printf("Processor-based splitting with ExternalBufferSink\n"); break;
	// TODO case 'r': printf("Processor-based splitting with RawFileSink\n"); break;
	default:  error("UNKNOWN Sink\n"); Ret = 1;
	}

	// mxflib Dictionaries must have been loaded globally
	if( !MDOType::IsDictLoaded() ) error("MXFLib Dictionaries not loaded\n");

	MXFFilePtr F = new MXFFile;
	if( !F->Open( file, true ) ) error("File %s failed to open\n", file );

	SplitProcessorPtr sp = SplitProcessor::Create( F );
	if ( !sp ) error("SplitProcessor failed to Create\n" );

	int clips = sp->Initialize();
	if( clips != 1 ) error("Can accept only single-container files\n");

	int available = sp->Discover();
	if( !available ) error("No Descriptors discovered\n");

	int connected = 0;
	for( int s = 0; s<available; s++ )
	{
		DescriptorPtr d = sp->GetDescriptor( s );

		if( Wanted( d ) )
		{
			EssenceSinkPtr esp;
			// instantiate appropriate subclass to receive essence data
			if     (ProcSink=='d')	esp = new DataChunkSink( );
			else if(ProcSink=='e')	esp = new ExternalBufferSink( NULL,0 );
			// TODO else if(ProcSink=='r')	esp = new RawFileSink( NULL );

			bool ok = sp->Connect( esp );

			if( !ok ) error("Failed to connect Descriptor to EssenceSink\n");
			else connected++;
		}
	}

	if( !connected ) error("No Descriptors connected\n");
	else			 printf("%d Descriptors connected\n", connected );

	if( !sp->Start() ) error("Start failed\n");

	while( sp->Next() );

	F->Close();
	return Ret;
}
