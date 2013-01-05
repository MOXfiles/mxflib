/*! \file	BASESINK.h
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
#ifndef _BASESINK_H_
#define _BASESINK_H_

#include "mxflib/mxflib.h"
using namespace mxflib;

#include "IPartial.h"
#include "streamfile.h"

namespace mxflib
{
	class EssenceSink_GCReadHandler : public GCReadHandler_Base
	{
	protected:
		EssenceSinkPtr _Sink;			//!< set during construction
		UInt32 _Count;

	private:
		EssenceSink_GCReadHandler();	//!< prevent default construction

	public:
		//! Construct a handler for a specified TrackNumber and thus EssenceSink
		EssenceSink_GCReadHandler( EssenceSinkPtr Sink ){ _Sink = Sink; _Count=0; }

		//! Handle a "chunk" of data that has been read from the file
		/*! \return true if all OK, false on error 
		 */
		virtual bool HandleData(GCReaderPtr Caller, KLVObjectPtr Object)
		{
			// debug
			printf( "%4d %08x %6d ", _Count++, Object->GetGCTrackNumber(), Object->GetLength() );

			size_t Size = Object->ReadData();
			bool Ret = _Sink->PutEssenceData( Object->GetData(), false ); 
			return Ret;
		};
	};
}

namespace mxflib
{
	//! EssenceSink that accepts a DataChunk from the source and copies it into a fresh buffer
	class DataChunkSink : public EssenceSink
	{
	protected:
		DataChunkPtr myChunk;					//!< the DataChunk with the buffer we have been given
		bool EndCalled;							//!< True once EndOfData is called

	public:
		//! Construct with required header values
		DataChunkSink() 
		{
			EndCalled = false;
		};

		//! Clean up
		virtual ~DataChunkSink() 
		{
			if(!EndCalled) EndOfData();
		};

		//! Receive the next "installment" of essence data
		/*! This will receive a buffer containing the next bytes of essence data
		 *  \param Buffer The data buffer
		 *  \param BufferSize The number of bytes in the data buffer
		 *  \param EndOfItem This buffer is the last in this wrapping item
		 *  \return True if all is OK, else false
		 *  \note The first call may well fail if the sink has not been fully configured.
		 *	\note If false is returned the caller should make no more calls to this function, but the function should be implemented such that it is safe to do so
		 */
		virtual bool PutEssenceData(UInt8 const *Buffer, size_t BufferSize, bool EndOfItem = true)
		{
			if( !myChunk )  myChunk = new DataChunk( BufferSize, Buffer );
			else			myChunk->Set( BufferSize, Buffer, 0 );

			// debug
			printf( "%6d\n", BufferSize );

			if( EndOfItem ) EndOfData();

			return true;
		}

		//! Called once all data exhausted
		/*! \return true if all is OK, else false
		 *  \note This function must also be called from the derived class' destructor in case it is never explicitly called
		 */
		virtual bool EndOfData(void) { EndCalled = true; return true; }
	};

	//! EssenceSink that rides upon an external buffer
	class ExternalBufferSink : public DataChunkSink
	{
	protected:
		DataChunkPtr myChunk;					//!< the DataChunk with the buffer we have been given
		bool EndCalled;							//!< True once EndOfData is called

	private:
		//! Prevent default construction
		ExternalBufferSink();

	public:
		//! Construct with required header values
		ExternalBufferSink( UInt8 *Buffer, size_t BuffSize ) 
		{
			if( !myChunk ) myChunk = new DataChunk;

			myChunk->SetBuffer( Buffer, BuffSize );
			EndCalled = false;
		};

		//! Clean up
		virtual ~ExternalBufferSink() 
		{
			if(!EndCalled) EndOfData();
		};

		//! Receive the next "installment" of essence data
		/*! This will receive a buffer containing the next bytes of essence data
		 *  \param Buffer The data buffer
		 *  \param BufferSize The number of bytes in the data buffer
		 *  \param EndOfItem This buffer is the last in this wrapping item
		 *  \return True if all is OK, else false
		 *  \note The first call may well fail if the sink has not been fully configured.
		 *	\note If false is returned the caller should make no more calls to this function, but the function should be implemented such that it is safe to do so
		 */
		virtual bool PutEssenceData(UInt8 const *Buffer, size_t BufferSize, bool EndOfItem = true)
		{
			if( !myChunk || myChunk->Size < BufferSize ) return false;
			else			myChunk->Set( BufferSize, Buffer, 0 );

			if( EndOfItem ) EndOfData();

			return true;
		}

		//! Called once all data exhausted
		/*! \return true if all is OK, else false
		 *  \note This function must also be called from the derived class' destructor in case it is never explicitly called
		 */
		virtual bool EndOfData(void) { EndCalled = true; return true; }
	};

	//! EssenceSink that writes a raw file to the currently open file
	class RawFileSink : public EssenceSink
	{
	protected:
		FileHandle File;						//!< The file to write
		StreamFile *Stream;						//!< The stream file info block
		bool EndCalled;							//!< True once EndOfData is called

	private:
		// Prevent default construction
		RawFileSink();

	public:
		// Construct with required header values
		RawFileSink(StreamFile *Stream) : Stream(Stream) 
		{
			File = FileInvalid;
			EndCalled = false;
		};

		//! Clean up
		virtual ~RawFileSink() 
		{
			if(!EndCalled) EndOfData();
		};

		//! Receive the next "installment" of essence data
		/*! This will receive a buffer containing the next bytes of essence data
		 *  \param Buffer The data buffer
		 *  \param BufferSize The number of bytes in the data buffer
		 *  \param EndOfItem This buffer is the last in this wrapping item
		 *  \return True if all is OK, else false
		 *  \note The first call may well fail if the sink has not been fully configured.
		 *	\note If false is returned the caller should make no more calls to this function, but the function should be implemented such that it is safe to do so
		 */
		virtual bool PutEssenceData(UInt8 const *Buffer, size_t BufferSize, bool EndOfItem = true)
		{
			// Set and open the file if not yet yet set
			if((!FileValid(File)) && Stream) File = Stream->GetFile();

			// Write the buffer, returning true if all the bytes were written
			return BufferSize == FileWrite(File, Buffer, BufferSize);
		}

		//! Called once all data exhausted
		/*! \return true if all is OK, else false
		 *  \note This function must also be called from the derived class' destructor in case it is never explicitly called
		 */
		virtual bool EndOfData(void) { EndCalled = true; return true; }
	};


	//! Class to allow percent done to be shown during extraction - passes all data to another sink
	class ShowPercentSink : public EssenceSink, public IPartial
	{
	private:
		ShowPercentSink();								//!< Prevent default construction

	protected:
		EssenceSinkPtr Sink;							//!< The EssenceSink to do the actual processing
		MXFFile *SourceFile;							//!< Source file being processed
		Length FileSize;								//!< Size of the file being processed

	public:
		//! Initialize this sink
		ShowPercentSink(EssenceSinkPtr TargetSink, MXFFile *File, Length Size = -1) : Sink(TargetSink), SourceFile(File)
		{
			// Work out the file size if not given
			if(Size == -1)
			{
				Position Pos = File->Tell();
				FileSize = File->SeekEnd();
				File->Seek(Pos);
			}
			else
				FileSize = Size;
		}

		//! Receive the next "installment" of essence data
		/*! This will receive a buffer containing the next bytes of essence data
		 *  \param Buffer The data buffer
		 *  \param BufferSize The number of bytes in the data buffer
		 *  \param EndOfItem This buffer is the last in this wrapping item
		 *  \return True if all is OK, else false
		 *  \note The first call may well fail if the sink has not been fully configured.
		 *	\note If false is returned the caller should make no more calls to this function, but the function should be implemented such that it is safe to do so
		 */
		virtual bool PutEssenceData(UInt8 const *Buffer, size_t BufferSize, bool EndOfItem = true)
		{
			static char msg[50];
			Position Pos = SourceFile->Tell();

			sprintf(msg,"%%%d\n",(int)((Pos*100)/FileSize));
			fputs(msg,stderr);
			fflush(stderr);

			return Sink->PutEssenceData(Buffer, BufferSize, EndOfItem);
		}

		//! Called once all data exhausted
		/*! \return true if all is OK, else false
		 *  \note This function must also be called from the derived class' destructor in case it is never explicitly called
		 */
		virtual bool EndOfData(void) { return Sink->EndOfData(); }


		/* IPartial */

		//! Has this partial clip has ended
		bool EndOfPartial(void)
		{
			// If our source is a partial source, hand over to it
			PartialSink *Partial = dynamic_cast<PartialSink *>(Sink.GetPtr());
			if(Partial) return Partial->EndOfPartial();
			return false;
		}
	};


	//! Sink that simply prepends a fixed byte to the start of each packet
	class PrependSink : public RawFileSink
	{
	protected:
		UInt8 Prepend1;							//!< Value to pre-pend to each packet
		UInt8 Prepend2;							//!< Value to pre-pend to each packet

	public:
		PrependSink(UInt8 Prepend1, UInt8 Prepend2, StreamFile *Stream) : RawFileSink(Stream), Prepend1(Prepend1), Prepend2(Prepend2)
		{
		}

		//! Receive the next "installment" of essence data
		/*! This will receive a buffer containing the next bytes of essence data
		 *  \param Buffer The data buffer
		 *  \param BufferSize The number of bytes in the data buffer
		 *  \param EndOfItem This buffer is the last in this wrapping item
		 *  \return True if all is OK, else false
		 *  \note The first call may well fail if the sink has not been fully configured.
		 *	\note If false is returned the caller should make no more calls to this function, but the function should be implemented such that it is safe to do so
		 */
		virtual bool PutEssenceData(UInt8 const *Buffer, size_t BufferSize, bool EndOfItem = true)
		{
			const UInt8 Prepend0 = 0xff;

			// Set and open the file if not yet yet set
			if((!FileValid(File)) && Stream) File = Stream->GetFile();

			// Prepend the bytes
			FileWrite(File, &Prepend0, 1);
			FileWrite(File, &Prepend1, 1);
			FileWrite(File, &Prepend2, 1);

			// Write the buffer via the original sink
			return RawFileSink::PutEssenceData(Buffer, BufferSize, EndOfItem);
		}
	};

	//! Sink that simply prepends a fixed byte and the length of data (as a byte) to the start of each packet
	class PrependLenSink : public PrependSink
	{
	public:
		PrependLenSink(UInt8 Prepend1, UInt8 Prepend2, StreamFile *Stream) : PrependSink(Prepend1, Prepend2, Stream)
		{
		}

		//! Receive the next "installment" of essence data
		/*! This will receive a buffer containing the next bytes of essence data
		 *  \param Buffer The data buffer
		 *  \param BufferSize The number of bytes in the data buffer
		 *  \param EndOfItem This buffer is the last in this wrapping item
		 *  \return True if all is OK, else false
		 *  \note The first call may well fail if the sink has not been fully configured.
		 *	\note If false is returned the caller should make no more calls to this function, but the function should be implemented such that it is safe to do so
		 */
		virtual bool PutEssenceData(UInt8 const *Buffer, size_t BufferSize, bool EndOfItem = true)
		{
			const UInt8 Prepend0 = 0xff;

			// Set and open the file if not yet yet set
			if((!FileValid(File)) && Stream) File = Stream->GetFile();

			// Prepend the byte
			FileWrite(File, &Prepend0, 1);
			FileWrite(File, &Prepend1, 1);
			FileWrite(File, &Prepend2, 1);

			// Prepend the length byte
			UInt8 Len = static_cast<UInt8>(BufferSize);
			FileWrite(File, &Len, 1);

			// Write the buffer via the original sink
			return RawFileSink::PutEssenceData(Buffer, BufferSize, EndOfItem);
		}
	};

}
#endif // _BASESINK_H_
