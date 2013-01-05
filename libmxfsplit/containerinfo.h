/*! \file	containerinfo.h
 *	\brief	
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
#ifndef _CONTAINERINFO_H_
#define _CONTAINERINFO_H_

#include "mxflib/mxflib.h"
using namespace mxflib;

namespace mxflib
{
	//! Map of Descriptors indexed by Track Number
	typedef std::map<UInt32, MDObjectPtr> TrackDescriptorMap;

	//! Map of EssenceSinks indexed by Track Number
	typedef std::map<UInt32, EssenceSinkPtr> EssenceSinkMap;

	//! Structure holding information about the essence in each Container
	class EssenceTrackDescriptor : public RefCount<EssenceTrackDescriptor>
	{
	public:
		UMIDPtr PackageID;									//!< The UMID for this BodySID
		PackagePtr Package;									//!< The package for this BodySID
		MDObjectPtr Descriptor;								//!< The main descriptor for this package (could be a multiple descriptor)

		TrackDescriptorMap TrackNumDescriptors;				//!< Map of descriptors for each TrackNumber
		TrackDescriptorMap TrackIDDescriptors;				//!< Map of descriptors for each TrackID - will create dummies if necessary

		EssenceSinkMap TrackNumSinks;						//!< Map of EssenceSinks for each TrackNumber

		//! Get the descriptor for a given track number - or the main descriptor if the track number is not associated with a descriptor
		MDObjectPtr GetDescriptor(UInt32 TrackNumber)
		{
			std::map<UInt32, MDObjectPtr>::iterator it = TrackNumDescriptors.find(TrackNumber);
			if(it != TrackNumDescriptors.end()) return (*it).second;
			return Descriptor;
		}

		//! Add the sole Descriptor to the map
		void AddSoleDescriptor(PackagePtr &Package, MDObjectPtr Descriptor);

		//! Build a map of track number to descriptor where the descriptor is a multiple descriptor
		void FillDescriptorMap(PackagePtr &Package, MDObjectPtr Descriptor);

	};

	//! Smart pointer to an EssenceTrackDescriptor object
	typedef SmartPtr<EssenceTrackDescriptor> EssenceTrackDescriptorPtr;

	//! Map of EssenceTrackDescriptor structures indexed by BodySID
	typedef std::map<UInt32, EssenceTrackDescriptorPtr> EssenceTrackDescriptorMap;

	//! Information object holding data about each essence stream
	class ContainerInfo : public RefCount<ContainerInfo>
	{
	public:
		//! Build a ContainerInfo for the essence in a given file
		/*! \return NULL on error
		 */
		static ContainerInfo* CreateAndBuild(MXFFilePtr &File);

	public:
		//! The header metadata for the partition used to extract the info - required here to keep all objects alive
		MetadataPtr HMeta;

		//! The map of essence info for this file
		EssenceTrackDescriptorMap Lookup;
	};

	//! Smart pointer to a ContainerInfo object
	typedef SmartPtr<ContainerInfo> ContainerInfoPtr;
}

#endif // _CONTAINERINFO_H_
