/*! \file	crypto_asdcp.h
 *	\brief	Definitions for AS-DCP compatible encryption and decryption
 *
 *	\version $Id$
 *
 */
/*
 *  Copyright (c) 2004, Matt Beard
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

#include <stdlib.h>

// Include AES encryption from OpenSSL
#include "openssl/aes.h"
#include "openssl/sha.h"


//! True if we are doing hashing calculations
extern bool Hashing;

//! Flag set if forcing a given key irrespective of the key details in the file
extern bool ForceKeyMode;


//! Build an AS-DCP hashing key from a given crypto key
/*  The hashing key is: 
 *  - trunc( HMAC-SHA-1( CipherKey, 0x00112233445566778899aabbccddeeff ) )
 *  Where trunc(x) is the first 128 bits of x
 */
DataChunkPtr BuildHashKey(int Size, const Uint8 *CryptoKey);

//! Build an AS-DCP hashing key from a given crypto key
/*  The hashing key is: 
 *  - trunc( HMAC-SHA-1( CipherKey, 0x00112233445566778899aabbccddeeff ) )
 *  Where trunc(x) is the first 128 bits of x
 */
inline DataChunkPtr BuildHashKey(DataChunkPtr &CryptoKey) { return BuildHashKey(CryptoKey->Size, CryptoKey->Data); }

//! Build an AS-DCP hashing key from a given crypto key
/*  The hashing key is: 
 *  - trunc( HMAC-SHA-1( CipherKey, 0x00112233445566778899aabbccddeeff ) )
 *  Where trunc(x) is the first 128 bits of x
 */
inline DataChunkPtr BuildHashKey(DataChunk &CryptoKey) { return BuildHashKey(CryptoKey.Size, CryptoKey.Data); }


// ============================================================================
//! HMAC-SHA1 hash function wrapper
/*! Calculates an HMAC-SHA1 hash for given data
 */
// ============================================================================

class HashHMACSHA1 : public Hash_Base
{
protected:
	Uint8 KeyBuffer_i[64];					//!< Inner key buffer, holds key xor 0x36
	Uint8 KeyBuffer_o[64];					//!< Outer key buffer, holds key xor 0x5c

	SHA_CTX Context;						//!< Our SHA-1 context

	bool KeyInited;							//!< True once the key has been initialized
/*
FileHandle OutFile;
*/

public:
	//! Initialize this object
	HashHMACSHA1() : KeyInited(false) {}

	//! Set the key and start hashing
	/*  \return True if key is accepted
	 */
	bool SetKey(Uint32 Size, const Uint8 *Key);

	//! Add the given data to the current hash being calculated
	void HashData(Uint32 Size, const Uint8 *Data);

	//! Get the finished hash value
	DataChunkPtr GetHash(void);
};



// ============================================================================
//! AES encryption class
// ============================================================================
class AESEncrypt : public Encrypt_Base
{
protected:
	AES_KEY CurrentKey;
	Uint8 CurrentIV[16];

public:
	//! Set an encryption key
	/*! \return True if key is accepted
	 */
	bool SetKey(Uint32 KeySize, const Uint8 *Key) 
	{
		int Ret = AES_set_encrypt_key(Key, 128, &CurrentKey);

		// Return true only if key setting was OK
		return Ret ? false : true; 
	};

	//! Set an encryption Initialization Vector
	/*! \return False if Initialization Vector is rejected
	 *  \note Some crypto schemes, such as cypher block chaining, only require
	 *        the initialization vector to be set at the start - in these cases
	 *        Force will be set to true when the vector needs to be initialized,
	 *        and false for any other calls.  This allows different schemes to be
	 *        used with minimal changes in the calling code.
	 */
	bool SetIV(Uint32 IVSize, const Uint8 *IV, bool Force = false) 
	{ 
		if(!Force) return false;

		if(IVSize != 16)
		{
			error("IV for AES encryption must by 16 bytes, tried to use IV of size %d\n", IVSize);
			return false;
		}

		memcpy(CurrentIV, IV, 16);

		return true; 
	};

	//! Get the Initialization Vector that will be used for the next encryption
	/*! If called immediately after SetIV() with Force=true or SetIV() for a crypto
	 *  scheme that accepts each offered vector (rather than creating its own ones)
	 *  the result will be the vector offered in that SetIV() call.
	 */
	DataChunkPtr GetIV(void) 
	{
		return new DataChunk(16, CurrentIV);
	}

	//! Can this encryption system safely encrypt in place?
	/*! If BlockSize is 0 this function will return true if encryption of all block sizes can be "in place".
	 *  Otherwise the result will indicate whether the given blocksize can be encrypted "in place".
	 */
	bool CanEncryptInPlace(Uint32 BlockSize = 0) { return false; }

	//! Encrypt data bytes in place
	/*! \return true if the encryption is successful
	 */
	bool EncryptInPlace(Uint32 Size, Uint8 *Data) { return false; }

	//! Encrypt data and return in a new buffer
	/*! \return NULL pointer if the encryption is unsuccessful
	 */
	DataChunkPtr Encrypt(Uint32 Size, const Uint8 *Data);
};


// ============================================================================
//! Encrypting GCReader handler
// ============================================================================
class Encrypt_GCReadHandler : public GCReadHandler_Base
{
protected:
	Uint32 OurSID;									//!< The BodySID of this essence
	GCWriterPtr Writer;								//!< GCWriter to receive encrypted data
	UUIDPtr ContextID;								//!< The context ID UL for this encrypted essence

	DataChunk EncKey;								//!< The encryption key we will use

	Length PlaintextOffset;							//!< Plaintext offset to use when encrypting

private:
	Encrypt_GCReadHandler();						//!< Don't allow standard construction

public:
	//! Construct a handler for a specified BodySID
	Encrypt_GCReadHandler(GCWriterPtr Writer, Uint32 BodySID, UUIDPtr &ContextID, DataChunkPtr KeyID, std::string KeyFileName);

	//! Handle a "chunk" of data that has been read from the file
	/*! \return true if all OK, false on error 
	 */
	virtual bool HandleData(GCReaderPtr Caller, KLVObjectPtr Object);

	//! Set the Plaintext offset to use when encrypting
	void SetPlaintextOffset(Length Offset) { PlaintextOffset = Offset; }
};



// ============================================================================
//! Basic GCReader handler for filler
/*! Currently discards the filler so is not useful, but can be extended if required 
 */
// ============================================================================
class Basic_GCFillerHandler : public GCReadHandler_Base
{
protected:
	Uint32 OurSID;								//!< The BodySID of this essence
	GCWriterPtr Writer;							//!< GCWriter to receive encrypted data

private:
	Basic_GCFillerHandler();					//!< Don't allow standard construction

public:
	//! Construct a filler handler for a specified BodySID
	Basic_GCFillerHandler(GCWriterPtr Writer, Uint32 BodySID) : Writer(Writer), OurSID(BodySID) {};

	//! Handle a "chunk" of data that has been read from the file
	/*! \return true if all OK, false on error 
	 */
	virtual bool HandleData(GCReaderPtr Caller, KLVObjectPtr Object) { return true; }
};



// ============================================================================
//! AES decryption class
// ============================================================================
class AESDecrypt : public Decrypt_Base
{
protected:
	AES_KEY CurrentKey;
	Uint8 CurrentIV[16];

public:
	//! Set an encryption key
	/*! \return True if key is accepted
	 */
	virtual bool SetKey(Uint32 KeySize, const Uint8 *Key) 
	{
		int Ret = AES_set_decrypt_key(Key, 128, &CurrentKey);

		// Return true only if key setting was OK
		return Ret ? false : true; 
	};

	//! Set a decryption Initialization Vector
	/*! \return False if Initialization Vector is rejected
	 *  \note Some crypto schemes, such as cypher block chaining, only require
	 *        the initialization vector to be set at the start - in these cases
	 *        Force will be set to true when the vector needs to be initialized,
	 *        and false for any other calls.  This allows different schemes to be
	 *        used with minimal changes in the calling code.
	 */
	bool SetIV(Uint32 IVSize, const Uint8 *IV, bool Force = false)
	{ 
		if(!Force) return false;

		if(IVSize != 16)
		{
			error("IV for AES encryption must by 16 bytes, tried to use IV of size %d\n", IVSize);
			return false;
		}

		memcpy(CurrentIV, IV, 16);

		return true; 
	};

	//! Get the Initialization Vector that will be used for the next decryption
	/*! If called immediately after SetIV() with Force=true or SetIV() for a crypto
	 *  scheme that accepts each offered vector (rather than creating its own ones)
	 *  the result will be the vector offered in that SetIV() call.
	 */
	DataChunkPtr GetIV(void)
	{
		return new DataChunk(16, CurrentIV);
	}

	//! Can this decryption system safely decrypt in place?
	/*! If BlockSize is 0 this function will return true if decryption of all block sizes can be "in place".
	 *  Otherwise the result will indicate whether the given blocksize can be decrypted "in place".
	 */
	bool CanDecryptInPlace(Uint32 BlockSize = 0) { return false; }

	//! Decrypt data bytes in place
	/*! \return true if the decryption <i>appears to be</i> successful
	 */
	bool DecryptInPlace(Uint32 Size, Uint8 *Data) { return false; }

	//! Decrypt data and return in a new buffer
	/*! \return true if the decryption <i>appears to be</i> successful
	 */
	DataChunkPtr Decrypt(Uint32 Size, const Uint8 *Data);
};



// ============================================================================
//! Decrypting GCReader encryption handler
// ============================================================================
class Decrypt_GCEncryptionHandler : public GCReadHandler_Base
{
protected:
	Uint32 OurSID;										//!< The BodySID of this essence

	DataChunk DecKey;									//!< The decryption key we will use

private:
	Decrypt_GCEncryptionHandler();						//!< Don't allow standard construction

public:
	//! Construct a handler for a specified BodySID
	Decrypt_GCEncryptionHandler(Uint32 BodySID, DataChunkPtr KeyID, std::string KeyFileName);
	//! Handle a "chunk" of data that has been read from the file

	/*! \return true if all OK, false on error 
	 */
	virtual bool HandleData(GCReaderPtr Caller, KLVObjectPtr Object);

	//! Determin if a valid key has been set
	bool KeyValid(void) { return (DecKey.Size == 16); }
};


// ============================================================================
//! Decrypting GCReader handler
/*! Passes data straight through to the output file - either decrypted by 
 *  Decrypt_GCEcryptionHandler or not encrypted in the source file
 */
// ============================================================================
class Decrypt_GCReadHandler : public GCReadHandler_Base
{
protected:
	Uint32 OurSID;								//!< The BodySID of this essence
	GCWriterPtr Writer;							//!< GCWriter to receive dencrypted data

private:
	Decrypt_GCReadHandler();						//!< Don't allow standard construction

public:
	//! Construct a test handler for a specified BodySID
	Decrypt_GCReadHandler(GCWriterPtr Writer, Uint32 BodySID) : Writer(Writer), OurSID(BodySID) {};

	//! Handle a "chunk" of data that has been read from the file
	/*! \return true if all OK, false on error 
	 */
	virtual bool HandleData(GCReaderPtr Caller, KLVObjectPtr Object) 
	{
		// Write the data without further processing
		Writer->WriteRaw(Object);
		return true;
	}
};
