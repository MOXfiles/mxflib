/*! \file	datachunk.cpp
 *	\brief	Simple re-sizable data chunk object
 *
 *	\version $Id$
 *
 */
/*
 *	Copyright (c) 2004, Matt Beard
 *
 *	This software is provided 'as-is', without any express or implied warranty.
 *	In no event will the authors be held liable for any damages arising from
 *	the use of this software.
 *
 *	Permission is granted to anyone to use this software for any purpose,
 *	including commercial applications, and to alter it and redistribute it
 *	freely, subject to the following restrictions:
 *
 *	  1. The origin of this software must not be misrepresented; you must
 *	     not claim that you wrote the original software. If you use this
 *	     software in a product, an acknowledgment in the product
 *	     documentation would be appreciated but is not required.
 *	
 *	  2. Altered source versions must be plainly marked as such, and must
 *	     not be misrepresented as being the original software.
 *	
 *	  3. This notice may not be removed or altered from any source
 *	     distribution.
 */

#include <mxflib/mxflib.h>

using namespace mxflib;



void mxflib::DataChunk::Resize(Uint32 NewSize)
{
	if(Size == NewSize) return;

	// Simple truncate or resize within the existing buffer size
	if(DataSize >= NewSize) 
	{
		Size = NewSize;
		return;
	}

	Uint32 AllocSize = NewSize;
	if(AllocationGranularity)
	{
//debug("AllocGran = %u, NewSize = %u, ", AllocationGranularity, NewSize);
		// Apply allocation granularity
		AllocSize = (NewSize-1) / AllocationGranularity;
//debug("AllocSize = %u, ", AllocSize);
		AllocSize = (AllocSize+1) * AllocationGranularity;
//debug("AllocSize = %u\n", AllocSize);
	}

	Uint8 *NewData = new Uint8[AllocSize];
	if(Size) memcpy(NewData, Data, Size);
	
//debug("Changing Buffer @ 0x%08x -> 0x%08x (0x%04x)\n", (int)Data, (int)NewData, (int)AllocSize);
	if((!ExternalBuffer) && (Data)) delete[] Data;
	ExternalBuffer = false;
	
	Data = NewData;
	DataSize = AllocSize;
	Size = NewSize;
}


//! Resize the data buffer, preserving contents
/*! The buffer is resized to <b>at least</b> NewSize, but Size remains unchanged */
void mxflib::DataChunk::ResizeBuffer(Uint32 NewSize)
{
	if(DataSize >= NewSize) return;

	if(AllocationGranularity)
	{
		// Apply allocation granularity
		NewSize = (NewSize-1) / AllocationGranularity;
		NewSize = (NewSize+1) * AllocationGranularity;
	}

	Uint8 *NewData = new Uint8[NewSize];
	if(Size) memcpy(NewData, Data, Size);

//debug("Changing Buffer @ 0x%08x -> 0x%08x (0x%04x)+\n", (int)Data, (int)NewData, (int)NewSize);
	if((!ExternalBuffer) && (Data)) delete[] Data;
	ExternalBuffer = false;
	
	Data = NewData;
	DataSize = NewSize;
}


//! Get a (hex) string representation of the data in the buffer
std::string mxflib::DataChunk::GetString(void)
{
	std::string Ret;
	unsigned i;
	for(i=0; i<Size; i++) 
	{
		if(i != 0) Ret += " ";
		Ret += Int2HexString(Data[i], 2);
	}

	return Ret;
};


//! Set an external buffer as the data buffer
/*! \note If an external buffer has been set for a DataChunk it may not
 *		  stay as the buffer in use. This is because there may not be
 *		  enough room in the buffer to hold new data. Therefore it is
 *		  important that the value of property <tt><b>Data</b></tt> is checked
 *		  before assuming the external buffer is still in use.
 */
void mxflib::DataChunk::SetBuffer(Uint8 *Buffer, Uint32 BuffSize, Uint32 AllocatedSize /*=0*/)
{
//debug("Setting Buffer @ 0x%08x -> 0x%08x\n", (int)Data, (int)Buffer);
	if((!ExternalBuffer) && (Data)) delete[] Data;

	Size = BuffSize;
	Data = Buffer;

	if(AllocatedSize == 0) DataSize = BuffSize;
	else DataSize = AllocatedSize;

	ExternalBuffer = true;
}


//! Transfer ownership of a data buffer from another DataChunk
/*! This is a very efficient way to set one DataChunk to the value of another.
 *  However it partially destroys the source DataChunk by stealing its buffer.
 *  \return true on success, false on failure
 */
bool mxflib::DataChunk::TakeBuffer(DataChunk &OldOwner)
{
	Uint32 BuffSize = OldOwner.Size;
	Uint32 AllocatedSize = OldOwner.DataSize;

	// Steal the old buffer
	Uint8 *Buffer = OldOwner.StealBuffer();

	// Fail if the old owner does not own its buffer!
	if(!Buffer) return false;

	SetBuffer(Buffer, BuffSize, AllocatedSize);

	return true;
}


//! Transfer ownership of a data buffer from another DataChunk (via a smart pointer)
/*! This is a very efficient way to set one DataChunk to the value of another.
 *  However it partially destroys the source DataChunk by stealing its buffer.
 *  \return true on success, false on failure
 */
bool mxflib::DataChunk::TakeBuffer(DataChunkPtr &OldOwner)
{
	Uint32 BuffSize = OldOwner->Size;
	Uint32 AllocatedSize = OldOwner->DataSize;

	// Steal the old buffer
	Uint8 *Buffer = OldOwner->StealBuffer();

	// Fail if the old owner does not own its buffer!
	if(!Buffer) return false;

	SetBuffer(Buffer, BuffSize, AllocatedSize);

	return true;
}
