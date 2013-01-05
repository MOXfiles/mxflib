/*! \file	STREAMFILE.h
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
#ifndef _STREAMFILE_H_
#define _STREAMFILE_H_

#include "mxflib/mxflib.h"
using namespace mxflib;

std::string BuildFilename(std::string Pattern, UInt32 BodySID, UInt32 TrackNumber, std::string Extension);

namespace mxflib
{
	//! ListWriter - write to a sequential set of files
	class ListWriter : public ListOfFiles, public RefCount<ListWriter>
	{
		bool CurrentFileOpen;					//!< True if we have a file open for writing
		FileHandle CurrentFile;					//!< The current file being written

	public:
		//! Construct a ListWriter and optionally set a single source filename pattern
		ListWriter(std::string FileName = "") : ListOfFiles(FileName)
		{
			// Unless a specific range has been specified - continue forever
			if(((!FileList) && (RangeStart == -1))) FilesRemaining = -1;
			CurrentFileOpen = false;
		}

		//! Open the current file (any new-file handler will already have been called)
		/*! Required for ListOfFiles
		 *  \return true if file open succeeded
		 */
		bool OpenFile(void)
		{
			CurrentFile = FileOpenNew(CurrentFileName.c_str());
			CurrentFileOpen = FileValid(CurrentFile);
			return CurrentFileOpen;
		}

		//! Close the current file
		/*! Required for ListOfFiles */
		void CloseFile(void)
		{
			if(CurrentFileOpen) FileClose(CurrentFile);
			CurrentFileOpen = false;
		}

		//! Is the current file open?
		/*! Required for ListOfFiles */
		bool IsFileOpen(void) { return CurrentFileOpen; }

		//! Get the current file handle
		FileHandle GetCurrentFile(void) { return CurrentFile; }
	};

	//! Smart pointer to a ListWriter
	typedef SmartPtr<ListWriter> ListWriterPtr;

	// Forward declare
	class StreamFile;

	//! A smart pointer to a StreamFile object
	typedef SmartPtr<StreamFile> StreamFilePtr;

	//! A list of pointers to StreamFile objects
	typedef std::list<StreamFilePtr> StreamFileList;

	//! Map of StreamFile blocks, indexed by (BodySID << 40) + (TrackNumber << 8) + ( 0 normally, 1-255 for sub-streams )
	typedef std::map<UInt64, StreamFilePtr> FileMap;

	
	class StreamFileManager;

	//!< Info block about an output stream
	class StreamFile : public RefCount<StreamFile>
	{
	public:
		StreamFileManager *Manager;				//!< The StreamFileManager than manages us, and other related streams
		FileHandle File;						//!< The file handle of the output file receiving the data
		std::string Name;						//!< The name of the output file receiving the data
		std::string Pattern;					//!< The filename pattern used to build the output filename
		UInt32 BodySID;							//!< The BodySID of the container holding this essence stream
		UInt32 TrackNumber;						//!< The TrackNumber of this essence stream
		int SubNumber;							//!< The sub-number of this stream under a parent (0 if top-level)
		std::string Extension;					//!< The file extensions derived for this essence type's output file
		GCElementKind Kind;						//!< The GCElement Kind
		EssenceSinkPtr Sink;					//!< The sink that is handling this data
		StreamFileList SubFiles;				//!< Any sub-files that belong to this stream

	public:
		//! Construct a StreamFile
		StreamFile(StreamFileManager *pParent)
		{
			Manager = pParent;
			File = FileInvalid;
			SubNumber = 0;
		};

		//! Open the output file for this stream - including building the filename from the pattern
		/*! DRAGONS: The default extension is "~" so we know if it was omitted
		 *  \ret The pointer to this object so we can select the file while using this as a function parameter
		 */
		StreamFile *OpenFile(std::string Extension = "~");

		//! Select the file, but don't yet open it (will record the filename)
		/*! \ret The pointer to this object so we can select the file while using this as a function parameter */
		StreamFile *SelectFile(std::string Extension = "")
		{
			this->Extension = Extension;
			return this;
		}

		//! Get the file handle - opening the file if not yet open
		FileHandle GetFile(void)
		{
			if(!FileValid(File)) OpenFile();
			return File;
		}

		//! Add a sub-stream to this stream, but don't yet open it
		StreamFile *SelectSub(std::string Extension ="~");
	};



	//! Manager to manage output streams
	class StreamFileManager
	{
	public:
		//! Current ListWriter
		ListWriterPtr ListMan;

		//! Are we dividing output files with a list manager?
		bool DivideFiles;

	protected:
		FileMap StreamMap;						//!< Map of streams managed by this manager

	public:
		StreamFileManager(std::string Filename = "")
		{
			DivideFiles = false;
		}

		//! Set the filename or filename pattern for the output files (will start list writing)
		void SetFileName(std::string FileName)
		{
			DivideFiles = true;
			if(!ListMan) ListMan = new ListWriter(FileName);
			else ListMan->SetFileName(FileName);
		}

		//! Find the specified stream
		StreamFile *Find(UInt32 BodySID, UInt32 TrackNumber, int Sub = 0)
		{
			FileMap::iterator it = StreamMap.find( MakeIndex(BodySID, TrackNumber, Sub) );
			if(it != StreamMap.end()) 
			{
				return (*it).second;
			}
			return NULL;
		}

		//! Add a primary or sub stream to the map
		StreamFile *AddStream(UInt32 BodySID, UInt32 TrackNumber, int Sub, GCElementKind Kind, std::string Pattern, std::string Extension = "")
		{
			StreamFilePtr Ret = MakeStream(BodySID, TrackNumber, Sub, Kind, Pattern, Extension);
			StreamMap[MakeIndex(BodySID, TrackNumber, Sub)] = Ret;
			return Ret;
		}

		//! Add a primary stream to the map
		StreamFile *AddStream(UInt32 BodySID, UInt32 TrackNumber, GCElementKind Kind, std::string Pattern, std::string Extension = "")
		{
			return AddStream(BodySID, TrackNumber, 0, Kind, Pattern, Extension);
		}

		//! Make a primary or sub stream
		StreamFilePtr MakeStream(UInt32 BodySID, UInt32 TrackNumber, int Sub, GCElementKind Kind, std::string Pattern, std::string Extension = "")
		{
			StreamFilePtr Ret = new StreamFile(this);

			Ret->BodySID = BodySID;
			Ret->TrackNumber = TrackNumber;
			Ret->SubNumber = Sub;
			Ret->Kind = Kind;
			Ret->Pattern = Pattern;
			Ret->Extension = Extension;

			StreamMap[MakeIndex(BodySID, TrackNumber, Sub)] = Ret;

			return Ret;
		}

		//! Make a primary stream
		StreamFilePtr MakeStream(UInt32 BodySID, UInt32 TrackNumber, GCElementKind Kind, std::string Pattern, std::string Extension = "")
		{
			return MakeStream(BodySID, TrackNumber, 0, Kind, Pattern, Extension);
		}

		//! Close any open files and clear the list
		void Close(void)
		{
			FileMap::iterator it = StreamMap.begin();
			while( StreamMap.end() != it )
			{
				if(DivideFiles)
				{
					if(ListMan) ListMan->CloseFile();
				}
				else
				{
					FileHandle F = (*it).second->File;

					if( FileValid(F) )
					{
						printf( "Closing %s, size 0x%s\n", (*it).second->Name.c_str(), Int64toHexString( FileTell( F ) ).c_str() );
						if((*it).second->Sink) (*it).second->Sink->EndOfData();
						FileClose( F );
					}
				}

				it++;
			}
			StreamMap.clear();
		}

	protected:
		UInt64 MakeIndex(UInt32 BodySID, UInt32 TrackNumber, int Sub)
		{
			return (static_cast<UInt64>(BodySID) << 40) + (static_cast<UInt64>(TrackNumber) << 8) + static_cast<UInt8>(Sub);
		}
	};

	//! Open the output file for this stream - including building the filename from the pattern
	/*! DRAGONS: The default extension is "~" so we know if it was omitted
	 *  \ret The pointer to this object so we can select the file while using this as a function parameter
	 */
	inline StreamFile *StreamFile::OpenFile(std::string Extension /*= "~"*/ )
	{
		// Open the file
		if(Manager->DivideFiles)
		{
			Manager->ListMan->GetNextFile();
			Name = "Sequence starting with " + Manager->ListMan->FileName();
			printf(" File=%s", Manager->ListMan->FileName().c_str());
			File = Manager->ListMan->GetCurrentFile();
		}
		else
		{
			Name = BuildFilename(Pattern, BodySID, TrackNumber, (Extension == "~") ? this->Extension : Extension);
			printf(" File=%s", Name.c_str());
			File = FileOpenNew(Name.c_str());
		}

		if( !FileValid(File) )
		{
			printf( " ERROR opening %s", Name.c_str());
		}

		return this;
	}

	
	//! Add a sub-stream to this stream, but don't yet open it
	inline StreamFile *StreamFile::SelectSub(std::string Extension /*="~"*/ )
	{
		StreamFilePtr Ret;
		Ret = Manager->AddStream(BodySID, TrackNumber, static_cast<int>(SubFiles.size()) + 1, Kind, Pattern, (Extension == "~") ? this->Extension : Extension);
		SubFiles.push_back(Ret);
		return Ret;
	};

}
#endif // _STREAMFILE_H_
