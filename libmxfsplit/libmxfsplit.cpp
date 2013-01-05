/*! \file	libmxfsplit.cpp
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

#include "libmxfsplit.h"

#include "mxflib/mxflib.h"
using namespace mxflib;

#include "containerinfo.h"

//! Initialize SplitProcessor
int SplitProcessor::Initialize()
{
	int Ret = 0;

	_ContainerInfo = ContainerInfo::CreateAndBuild(_File);
	if( !_ContainerInfo ) error("Failed to Build ContainerInfo\n");

	// the Body Reader used to read the file
	if( !_Reader ) _Reader = new BodyReader(_File);

	return _ContainerInfo->Lookup.size();
};

//! Choose Container and discover available Descriptors
int SplitProcessor::Discover( const int Ix /* = 0 */ )
{
	int Ret = 0;

	int i = Ix;
	EssenceTrackDescriptorMap::iterator it = _ContainerInfo->Lookup.begin();
	while((it != _ContainerInfo->Lookup.end()) && (i--)) it++;

	if( it == _ContainerInfo->Lookup.end() )
	{
		_Container = NULL;
		_Descriptor = NULL;
		return Ret;
	}

	_Container = (*it).second;
	_BodySID = (*it).first;
	_Reader->MakeGCReader( _BodySID );

	Ret = (int) _Container->TrackNumDescriptors.size();

	if( Ret )
	{
		// get the first track, by TrackID using the first entry in the map
		UInt32 tnum = (*(_Container->TrackNumDescriptors.begin())).first;

		TrackList::iterator it = _Container->Package->Tracks.begin();
		while(it != _Container->Package->Tracks.end())
		{
			if((*it)->GetTrackNumber() == tnum) break;
			it++;
		}

		TrackPtr t = *it;
		if( t )
		{
			_EditRate =	t->GetEditRate();
			_Length = t->GetDuration();
		}
		else
		{
			// will only happen if earlier steps failed
			_EditRate =	Rational(0,1);
			_Length = 0;
		}
	}
	else
	{
		_Descriptor = NULL;
	}

	_Container->TrackNumSinks.clear();

	return Ret;
};

//! Choose a Descriptor (no default) for the chosen Container
DescriptorPtr SplitProcessor::GetDescriptor( const int Ix )
{
	int i = Ix;
	TrackDescriptorMap::iterator it = _Container->TrackNumDescriptors.begin();
	while((it != _Container->TrackNumDescriptors.end()) && (i--)) it++;

	if( it == _Container->TrackNumDescriptors.end() )
	{
		_Descriptor = NULL;
	}

	_Descriptor = (*it).second;
	_DescriptorTrackNum = (*it).first;

	return _Descriptor;
};

//! Connect the stream associated with a Descriptor to an EssenceSink 
bool SplitProcessor::Connect( EssenceSinkPtr Sink )
{
	if( !_Descriptor ) return false;

	_Container->TrackNumSinks[_DescriptorTrackNum] = Sink;

	_Reader->GetGCReader( _BodySID )->SetDataHandler( _DescriptorTrackNum, new EssenceSink_GCReadHandler( Sink ) );

	return true;
};

//! Get EssenceSink for a stream associated with a Descriptor
EssenceSinkPtr SplitProcessor::GetSink( const int Ix )
{
	// DRAGONS index in TrackNumDescriptors because TrackNumSinks probably has different entries

	int i = Ix;
	TrackDescriptorMap::iterator it = _Container->TrackNumDescriptors.begin();
	while((it != _Container->TrackNumDescriptors.end()) && (i--)) it++;

	if( it == _Container->TrackNumDescriptors.end() ) return NULL;

	EssenceSinkMap::iterator is = _Container->TrackNumSinks.find( (*it).first );

	if( is == _Container->TrackNumSinks.end() ) return NULL;

	return (*is).second;
};

//! Set up BodyReader, Indexes, and Seek to (zero-based) position in Container
bool SplitProcessor::Start( Position Pos /* = 0 */ )
{
	bool Ret = false;

	// TODO use _ContainerInfo->HMeta
	// TODO create and scan _IndexMap

	// TODO _Reader->Seek( _BodySID, 0 );
	// TODO Seek(0)

	return Ret;
};

//! Seek to (zero-based) position in Container
bool SplitProcessor::Seek( Position Pos )
{
	bool Ret = false;

	// TODO use _IndexMap

	return Ret;
};

//! Move current position to Next edit unit in Container
bool SplitProcessor::Next()
{
	bool Ret = _Reader->ReadFromFile( true, Unit_GC );

	return Ret;
};
