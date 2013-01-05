/*! \file	AUDIOSINK.h
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
#ifndef _AUDIOSINK_H_
#define _AUDIOSINK_H_

#include "mxflib/mxflib.h"
using namespace mxflib;

#include "streamfile.h"

namespace mxflib
{
	//! EssenceSink that writes a wave file to the currently open file
	class WaveFileSink : public EssenceSink
	{
	protected:
		FileHandle File;						//!< The file to write
		StreamFile *Stream;						//!< The stream file info block
		unsigned int ChannelCount;				//!< The number of audio channels
        UInt32 SamplesPerSec;					//!< The sample rate in smaples per second
        unsigned int BitsPerSample;				//!< The number of bits per sample, per channel
		UInt32 DataSize;						//!< The size of the entire data chunk of the finished wave file (if known), else 0
		bool HeaderWritten;						//!< Set true once the wave header has been written
		bool EndCalled;							//!< True once EndOfData is called

	private:
		// Prevent default construction
		WaveFileSink();

	public:
		// Construct with required header values
		WaveFileSink(StreamFile *Stream, 
					unsigned int Channels, 
					UInt32 SamplesPerSec, 
					unsigned int BitsPerSample,
					UInt32 DataSize = 0)
					: Stream(Stream), 
					ChannelCount(Channels), SamplesPerSec(SamplesPerSec), 
					BitsPerSample(BitsPerSample), DataSize(DataSize)
		{
			File = FileInvalid;
			HeaderWritten = false;
			EndCalled = false;
		};

		//! Clean up
		virtual ~WaveFileSink()
		{
			if(!EndCalled) EndOfData();
		}

		//! Receive the next "installment" of essence data
		/*! This will recieve a buffer containing thhe next bytes of essence data
		 *  \param Buffer The data buffer
		 *  \param BufferSize The number of bytes in the data buffer
		 *  \param EndOfItem This buffer is the last in this wrapping item
		 *  \return True if all is OK, else false
		 *  \note The first call may well fail if the sink has not been fully configured.
		 *	\note If false is returned the caller should make no more calls to this function, but the function should be implemented such that it is safe to do so
		 */
		virtual bool PutEssenceData(UInt8 const *Buffer, size_t BufferSize, bool EndOfItem = true);

		//! Called once all data exhausted
		/*! \return true if all is OK, else false
		 *  \note This function must also be called from the derived class' destructor in case it is never explicitly called
		 */
		virtual bool EndOfData(void);
	};



}
#endif // _AUDIOSINK_H_
