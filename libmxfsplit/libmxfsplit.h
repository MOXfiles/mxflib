/*! \file	libmxfsplit.h
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
#ifndef _LIBMXFSPLIT_H_
#define _LIBMXFSPLIT_H_

#include "mxflib/mxflib.h"
using namespace mxflib;

#include "containerinfo.h"
#include "basesink.h"

#include <vector>

typedef MDObjectPtr DescriptorPtr;

namespace mxflib
{
	//! SplitProcessor - singleton class to do all processing of MXF file splitting
	class SplitProcessor: public RefCount<SplitProcessor>
	{
	private:
		//! Prevent default construction
		SplitProcessor();

	public:
		//! Construct a SplitProcessor on an MXFFile
		/*! 
		 *	\param	F The MXFFile to be processed
		 */
		SplitProcessor( MXFFilePtr F ) : _File(F) {};

		//! Create a SplitProcessor on an MXF file
		/*! 
		 *	\param	F The MXFFile to be processed
		 */
		static SplitProcessor* Create( MXFFilePtr F ){ return new SplitProcessor(F); }

		//! Initialize SplitProcessor
		/*! Initialize HMeta, Validate parseable, determine how many Containers within file
		 *  \return Number of Containers
		 *  \note	For op1a, number of Containers == 1
		 */
		int Initialize();

		//! Choose Container and discover available Descriptors
		/*! 
		 *	\param	Ix The (zero-based) index of the Container (default=0)
		 *  \return Number of Descriptors
		 *	\note	sets default _EditRate and _Length
		 */
		int Discover( const int Ix = 0 );

		//! Get EditRate of chosen Container
		/*! 
		 *  \return EditRate of first Descriptor in the Container, e.g. 30000/1001
		 *  \note	Per 377-1, all EssenceTracks of a TopLevel SourcePackage have the same EditRate
		 */
		Rational GetEditRate() { return _EditRate; }

		//! Get Length of chosen Container
		/*! 
		 *  \return Length of first Descriptor in the Container, measured in EditUnits
		 *  \note	Per 377-1, all EssenceTracks of a TopLevel SourcePackage have the same Length
		 */
		Position GetLength() { return _Length; }

		//! Choose a Descriptor (no default) for the chosen Container
		/*! 
		 *	\param	Ix The (zero-based) index of the Descriptor
		 *  \return Smart Pointer to Descriptor
		 *  \note	Descriptors are kept in a <map> by EditNumber - which matches the EssenceElementNumber in the Body
		 */
		DescriptorPtr GetDescriptor( const int Ix );

		//! Connect the stream associated with the selected Descriptor to an EssenceSink 
		/*! 
		 *	\param	Sink A Smart Pointer to a newly constructed EssenceSink
		 *  \return false if the connection was not made
		 */
		bool Connect( EssenceSinkPtr Sink );

		//! Get EssenceSink for a stream associated with a Descriptor
		/*! 
		 *	\param	Ix The (zero-based) index of the Descriptor
		 *  \return Smart Pointer to an EssenceSink
		 *	\note	Connect() must have been used to make the connection
		 */
		EssenceSinkPtr GetSink( const int Ix );

		//! Set up BodyReader, Indexes, and Seek to (zero-based) position in Container
		/*! 
		 *  \return false if the set up fails or this position does not exist
		 *	\note	upon success, use Next() to read data and iterate forwards
		 */
		bool Start( Position Pos = 0 );

		//! Seek to (zero-based) position in Container
		/*! 
		 *	\param	Pos The (zero-based) position in the stream, measured in EditUnits
		 *  \return false if this position does not exist
		 *	\note	upon success, use Next() to read data and iterate forwards
		 */
		bool Seek( Position Pos );

		//! Move current position to Next edit unit in Container
		/*! 
		 *  \return false if no more available
		 *	\note	Next() must be called after each Seek()
		 */
		bool Next();

		//! Destructor
		~SplitProcessor(){};

	private:
		MXFFilePtr _File;							//!< the file being processed

		ContainerInfoPtr _ContainerInfo;			//!< map of Containers and Descriptors built by Initialize

		EssenceTrackDescriptorPtr _Container;		//!< selected Container
		UInt32 _BodySID;							//<! BodySID of selected Container
		Rational _EditRate;							//!< default EditRate of selected Container
		Position _Length;							//!< default Length of selected Container

		DescriptorPtr _Descriptor;					//!< selected Descriptor
		UInt32 _DescriptorTrackNum;					//!< TrackNumber of selected Descriptor

		std::map<UInt32, IndexTablePtr> _IndexMap;	//!< all Index Tables found in the file (selected one will be in _Container

		BodyReaderPtr _Reader;						//!< BodyReader set by Initialize()
	};

	//! Smart pointer to a SplitProcessor
	typedef SmartPtr<SplitProcessor> SplitProcessorPtr;

} // namespace mxflib

#endif // _LIBMXFSPLIT_H_
