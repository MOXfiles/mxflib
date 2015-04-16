

#include "audiosink.h"


//! Write a basic wave fle header
static void WriteWaveHeader(FileHandle File, Int16 Channels, UInt32 SamplesPerSec, UInt16 BitsPerSample, UInt32 DataSize /*=0*/)
{
	const UInt32 ID_RIFF = 0x52494646;		//! "RIFF"
	const UInt32 ID_WAVE = 0x57415645;		//! "WAVE"
	const UInt32 ID_fmt  = 0x666d7420;		//! "fmt "
	const UInt32 ID_data = 0x64617461;		//! "data"

	//! Buffer big enough to hold a basic Wave Header
	UInt8 Buffer[44];

	/*  The format written is as follows:

		fourcc		fRIFF;				// 0
		LEUInt32	RIFF_len;			// 4
		fourcc		fWAVE;				// 8

		fourcc		ffmt_;				// 12
		LEUInt32	fmt__len;			// 16

		LEUInt16	format;				// 20
		LEUInt16	nchannels;			// 22
		LEUInt32	samplespersec;		// 24
		LEUInt32	avgbps;				// 28
		LEUInt16	blockalign;			// 32
		LEUInt16	bitspersample;		// 34

		fourcc		data;				// 36
		LEUInt32	data_len;			// 40
										// 44
	*/
	//! Walking buffer pointer
	UInt8 *p = Buffer;
	// Write the initial RIFF FourCC
	PutU32(ID_RIFF, p);
	p+= 4;

	// Write the length of the file with only the header (excluding the first 8 bytes)
	PutU32_LE(38 + DataSize, p);
	p+= 4;

	// Write the WAVE FourCC
	PutU32(ID_WAVE, p);
	p+= 4;

	// Write the fmt_ FourCC
	PutU32(ID_fmt, p);
	p+= 4;
	// And the length of the fmt_ chunk
	PutU32_LE(16, p);
	p+= 4;
	/* Write the format chunk */
	// AudioFormat = PCM
	PutU16_LE(1, p);
	p += 2;
	// NumChannels
	PutU16_LE(Channels, p);
	p += 2;

	// SamplesRate
	PutU32_LE(SamplesPerSec, p);
	p += 4;

	// ByteRate
	PutU32_LE((SamplesPerSec * Channels * BitsPerSample)/8, p);
	p += 4;

	// BlockAlign
	PutU16_LE((Channels * BitsPerSample)/8, p);
	p += 2;

	// BitsPerSample
	PutU16_LE(BitsPerSample, p);
	p += 2;


	/* Write the data header */
	
	// Write the data FourCC
	PutU32(ID_data, p);
	p+= 4;
	// Write the length of the data
	PutU32_LE(DataSize, p);
	// Write this data to the file
	FileWrite(File, Buffer, 44);
}


//! Update the lengths in the header of the specified wave file
/*! \return true if updated OK */
static bool UpdateWaveLengths(FileHandle File)
{
	const UInt32 ID_RIFF = 0x52494646;		//! "RIFF"
	const UInt32 ID_WAVE = 0x57415645;		//! "WAVE"
	const UInt32 ID_fmt  = 0x666d7420;		//! "fmt "
	const UInt32 ID_data = 0x64617461;		//! "data"

	//! Buffer for working values
	UInt8 Buffer[20];

	// Determine the size of the file (Note it can not be > 4Gb)
	FileSeekEnd(File);
	UInt32 FileSize = (UInt32)FileTell(File);

	// Read the start of the header
	FileSeek(File, 0);
	if(FileRead(File, Buffer, 20) != 20) return false;

	// Check the initial RIFF FourCC
	if(GetU32(Buffer) != ID_RIFF) return false;

	// Check the WAVE FourCC
	if(GetU32(&Buffer[8]) != ID_WAVE) return false;

	// Check the fmt_ FourCC
	if(GetU32(&Buffer[12]) != ID_fmt) return false;

	// Get the length of the format chunk
	UInt32 FormatLength = GetU32_LE(&Buffer[16]);

	// Read the following chunk
	FileSeek(File, FormatLength + 20);
	if(FileRead(File, Buffer, 4) != 4) return false;

	// Check the data FourCC (doesn't have to be here for a valid wave file, but it's all we support!)
	if(GetU32(Buffer) != ID_data) return false;

	// Write the file length (less the first 8 bytes)
	PutU32_LE(FileSize - 8, Buffer);
	FileSeek(File, 4);
	FileWrite(File, Buffer, 4);

	// Write the file length (less the first 28 bytes)
	PutU32_LE(FileSize - (FormatLength + 28), Buffer);
	FileSeek(File, FormatLength + 24);
	FileWrite(File, Buffer, 4);

	return true;
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
bool WaveFileSink::PutEssenceData(UInt8 const *Buffer, size_t BufferSize, bool EndOfItem /*=true*/)
{
	if(!HeaderWritten)
	{
		WriteWaveHeader(File, static_cast<Int16>(ChannelCount), SamplesPerSec, static_cast<Int16>(BitsPerSample), DataSize);
		HeaderWritten = true;
	}

	// Write the buffer, returning true if all the bytes were written
	return BufferSize == FileWrite(File, Buffer, BufferSize);
}

//! Called once all data exhausted
/*! \return true if all is OK, else false
	*/
bool WaveFileSink::EndOfData(void)
{
	bool Ret = true;

	if(DataSize == 0)
	{
		// Update the length fields if required
		Ret = UpdateWaveLengths(File);
	}

	EndCalled = true;

	return Ret;
}


