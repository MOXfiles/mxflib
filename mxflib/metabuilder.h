
#ifndef MXFLIB__METABUILDER_H
#define MXFLIB__METABUILDER_H

#include <sstream>
#include <iomanip>


namespace mxflib
{
	/* Forward declare classes and pointers */

	class PackageInfo;

	//! Smart pointer to a PackageInfo
	typedef SmartPtr<PackageInfo> PackageInfoPtr;

	// Parent pointer to a PackageInfo
	typedef ParentPtr<PackageInfo> PackageInfoParent;

	//! List of smart pointers to a PackageInfo objects
	typedef std::list<PackageInfoPtr> PackageInfoList;


	class MetadataInfo;

	//! Smart pointer to a MetadataInfo
	typedef SmartPtr<MetadataInfo> MetadataInfoPtr;

	//! Parent pointer to a MetaInstanceInfo object
	typedef ParentPtr<MetadataInfo> MetadataInfoParent;

	//! List of smart pointers to a MetadataInfo objects
	typedef std::list<MetadataInfoPtr> MetadataInfoList;


	class TrackInfo;

	// Smart pointer to a TrackInfo
	typedef SmartPtr<TrackInfo> TrackInfoPtr;

	// Parent pointer to a TrackInfo
	typedef ParentPtr<TrackInfo> TrackInfoParent;

	//! List of smart pointers to a TrackInfo objects
	typedef std::list<TrackInfoPtr> TrackInfoList;

	
	class TimecodeTrackInfo;

	//! Smart pointer to a TimecodeTrackInfo
	typedef SmartPtr<TimecodeTrackInfo> TimecodeTrackInfoPtr;

	//! List of smart pointers to a TimecodeTrackInfo objects
	typedef std::list<TimecodeTrackInfoPtr> TimecodeTrackInfoList;


	//! Class holding information about a clip or component being built by a MetaBuilder
	class ClipInfo : public RefCount<ClipInfo>
	{
	public:
		TrackInfoParent Parent;					//!< The track owning this clip
		ComponentPtr Instance;					//!< Pointer to the actual instance, once built, before then NULL
		TrackInfoParent RefTrack;				//!< Pointer to the referenced source track, or NULL if the end of the ref chain
		Position Start;							//!< The start position of this source clip
		Length Duration;						//!< The duration of this clip, or -1 if unknown
		bool AutoDuration;						//!< Is the duration of this clip automatically adjusted (rather than specifically set)
												/*!< If auto adjusted, it will try and match any referenced track or linked essence */
	protected:
		bool InRecursion;						//!< Flag set true when recursing inside SetDuration - helps break infinite loop
												/*!< ClipInfo::SetDuration() calls TrackInfo::SetDuration() which can call ClipInfo::SetDuration
												 *   so ClipInfo::SetDuration() sets InRecursion and if it finds it again - errors */
	public:
		//! Simple constructor
		ClipInfo(TrackInfo *Parent) : Start(0), Duration(-1), AutoDuration(true), InRecursion(false) { this->Parent = Parent; }

		//! Constructor for clip referencing another track
		ClipInfo(TrackInfo *Parent, TrackInfo *RefTrack);

		//! Constructor for clip referencing another clip
		ClipInfo(TrackInfo *Parent, ClipInfo *RefClip);

		//! Constructor for clip referencing a specific section of another clip
		ClipInfo(TrackInfo *Parent, ClipInfo *RefClip, Position Start, Length Duration );

		//! Set the duration of this clip automatically (if using auto duration)
		Length SetDuration(void);

		//! Set the duration of this clip to a given value
		void SetDuration(Length Duration) { SetDuration(Duration, false); }
	
	protected:
		//! Set the duration of this clip to a given value, allowing the AutoDuration property to be left untouched if required
		void SetDuration(Length Duration, bool MaintainAuto);
	};

	//! Smart pointer to a ClipInfo
	typedef SmartPtr<ClipInfo> ClipInfoPtr;

	//! List of smart pointers to a ClipInfo objects
	typedef std::list<ClipInfoPtr> ClipInfoList;


	//! Class holding information about a timecode track being built by a MetaBuilder
	class TimecodeTrackInfo : public RefCount<TimecodeTrackInfo>
	{
	public:
		PackageInfoParent Parent;				//!< The package owning this track
		TrackPtr Instance;						//!< Pointer to the actual instance, once built, before then NULL
		UInt32 TrackNumber;						//!< The track number
		UInt32 TrackID;							//!< The track ID of this track
		UInt16 FPS;								//!< Integer FPS for the timecode, 0 for unknown
		bool DropFrame;							//!< DropFrame flag
		Position Start;							//!< The start timecode, as an edit unit offset from 00:00:00:00
		std::string StartString;				//!< If not empty, overrides the Start value
		// TODO: Add the ability for discontinuous timecode
		Length Duration;						//!< The duration of this continuous timecode track
		bool AutoDuration;						//!< Is the duration of this track automatically adjusted (rather than specifically set)
												/*!< If auto adjusted, it will try and match the other tracks in this package */

	public:
		//! Constructor for empty timecode track
		TimecodeTrackInfo(PackageInfo *Parent) 
			: TrackNumber(0), TrackID(0), FPS(0), DropFrame(false), Start(0), Duration(-1), AutoDuration(true) 
		{ 
			this->Parent = Parent; 
		}

		//! Set the duration of this track automatically (if using auto duration)
		Length SetDuration(void);

		//! Set the duration of this track to a given value
		void SetDuration(Length Duration) { SetDuration(Duration, false); }
	

		//! Build a timecode track info block from an existing track instance
		static TimecodeTrackInfoPtr Parse(PackageInfo *Parent, Track *Instance);

	protected:
		//! Set the duration of this track to a given value, allowing the AutoDuration property to be left untouched if required
		void SetDuration(Length Duration, bool MaintainAuto);

		//! Allocate a track ID to this timecode track
		UInt32 AllocateTrackID(void);

		// Allow our holder to call protected methods
		friend PackageInfo;
	};



	//! Class holding information about a track being built by a MetaBuilder
	class TrackInfo : public RefCount<TrackInfo>
	{
	public:
		PackageInfoParent Parent;				//!< The package owning this track
		TrackPtr Instance;						//!< Pointer to the actual instance, once built, before then NULL
		ClipInfoList Clips;						//!< List of all clips/components for this track
		Track::TrackType Type;					//!< The type of this track
		UInt32 TrackNumber;						//!< The track number for linking this track to internal essence
		UInt32 TrackID;							//!< The track ID of this track
		Position Origin;						//!< The origin value for this track
		EssenceSourcePtr Source;				//!< The source for this track, or NULL
		WrappingConfigPtr Config;				//!< The wrapping config for this track, or NULL
		bool AutoDuration;						//!< Is the duration of this track automatically adjusted (rather than specifically set)
												/*!< If auto adjusted, it will try and match any referenced track or linked essence */
		MDObjectPtr Descriptor;					//!< The descriptor object for this essence track, or NULL
		bool AutoDescriptorDuration;			//!< True if the descriptor is to have its duration set automatically to match this track

	protected:
		bool InRecursion;						//!< Flag set true when recursing inside GetEditRate - helps break infinite loop
												/*!< TrackInfo::GetEditRate() calls TrackInfo::GetEditRate()
												 *   so before doing so it sets InRecursion and if it finds it again - errors */
	public:
		//! Constructor for empty track
		TrackInfo(PackageInfo *Parent)
			: Type(Track::TrackTypeUndetermined), TrackNumber(0), TrackID(0), Origin(0), AutoDuration(true), AutoDescriptorDuration(true), InRecursion(false)
		{ 
			this->Parent = Parent; 
		}

		//! Constructor for essence track with specified source
		TrackInfo(PackageInfo *Parent, EssenceSource *Source, WrappingConfig *Config);

		//! Set the duration of this track automatically (if using auto duration)
		Length SetDuration(void);

		//! Set the duration of this track to a given value
		void SetDuration(Length Duration) { SetDuration(Duration, false); }
	
		//! Get the duration of this track from the clips
		Length GetDuration(void);

		//! Determine the edit rate for this track
		Rational GetEditRate(void);

		
		//! Build a track info block from an existing track instance
		static TrackInfoPtr Parse(PackageInfo *Parent, Track *Instance);

	protected:
		//! Set the duration of this track to a given value, allowing the AutoDuration property to be left untouched if required
		void SetDuration(Length Duration, bool MaintainAuto);

		//! Allocate a track ID to this track
		UInt32 AllocateTrackID(void);

		// Allow our holder to call protected methods
		friend PackageInfo;
	};



	//! Class holding information about a track being built by a MetaBuilder
	class PackageInfo : public RefCount<PackageInfo>
	{
	public:
		MetadataInfoParent Parent;				//!< The MetaInstanceInfo owning this package
		bool IsMaterial;						//!< True if this is a material package, false for source packages
		PackagePtr Instance;					//!< Pointer to the actual instance, once built, before then NULL
		TimecodeTrackInfoList TimecodeTracks;	//!< List of all timecode tracks for this package
		TrackInfoList EssenceTracks;			//!< List of all essence tracks for this package
		UMIDPtr PackageID;						//!< The UMID for this package
		UInt32 BodySID;							//!< The BodySID of this package (if internal), or zero
		UInt32 IndexSID;						//!< The IndexSID of this package (if internally indexed), or zero

	public:
		// Empty constructor
		PackageInfo(MetadataInfo *Parent, bool IsMaterial = false) : IsMaterial(IsMaterial), BodySID(0), IndexSID(0)
		{
			this->Parent = Parent;

			// Make a generic "type is not identified" UMID
			PackageID = MakeUMID(0x0f);
		}

		//! Constructor with a single internal essence track
		PackageInfo(MetadataInfo *Parent, UInt32 BodySID, EssenceSource *Source, WrappingConfig *Config);

		//! Add an essence track to this package for the given internal essence
		TrackInfo* AddEssenceTrack(EssenceSource *Source, WrappingConfig *Config);

		//! Add essence tracks that reference all essence tracks in a given package to this packages
		void AddEssenceTracks(PackageInfo *Package);

		//! Add an essence track to this package that references a given track (in another package)
		TrackInfo* AddEssenceTrack(TrackInfo *Track);

		//! Add one or more essence tracks linked to the specified source, limited to a specified number of channels per track
		TrackInfoList AddEssenceMultiTracks(EssenceSource *Source, WrappingConfig *Config, int ChannelsPerTrack = 1);

		//! Set the duration of every essence and timecode track to a given value
		void SetDurations(Length Duration);

		//! Auto set the duration of every essence and timecode track in this package
		void SetDurations(void);

		//! Build or update an instance of the package for this info block
		bool UpdateInstance(void);

		//! Get a pointer to the first essence track - such as the one added when constructing a package with an essence source
		TrackInfo* EssenceTrack(void) 
		{ 
			if(EssenceTracks.empty()) return NULL;
			return EssenceTracks.front();
		}

		//! Locate a track for a given track ID
		TrackInfo* LocateTrack(UInt32 TrackID);


		//! Build a package info block from an existing package instance
		static PackageInfoPtr Parse(MetadataInfo *Parent, Package *Instance);

	protected:
		//! Allocate a track ID to a given timecode track
		UInt32 AllocateTrackID(TimecodeTrackInfo *Track);

		//! Allocate a track ID to a given track
		UInt32 AllocateTrackID(TrackInfo *Track);

		// Allow our holder to call protected build commands
		friend class MetadataInfo;

		// Allow our contents to call our protected methods
		friend class TimecodeTrackInfo;
		friend class TrackInfo;
	};


	class MetaBuilder;

	//! Parent pointer to a MetaBuilder
	typedef ParentPtr<MetaBuilder> MetaBuilderParent;

	//! Class holding information about a metadata instance being built by a MetaBuilder
	class MetadataInfo : public RefCount<MetadataInfo>
	{
	public:
		MetadataPtr Instance;					//!< Pointer to the actual instance, once built, before then NULL
		PackageInfoList Packages;				//!< List of all packages for this instance
		PackageInfoPtr Master;					//!< The master material package that will automatically reference each file package, or NULL

	protected:
		MetaBuilderParent Parent;				//!< The MetaBuilder who built the instance, once built, before then NULL

		PackageInfo *pMaterialPackage;			//!< Pointer to the material package that is added automatically when constructing the instance
		PackageInfo *pFilePackage;				//!< Pointer to the file package that may be added when constructing the instance, NULL if none added

	public:
		//! Construct a basic MetaInstanceInfo
		MetadataInfo(MetaBuilder *Parent);

		//! Construct a new metadata instance, with a specified internal source
		MetadataInfo(MetaBuilder *Parent, UInt32 BodySID, EssenceSourcePtr &Source, WrappingConfigPtr &Config);

		//! Add another material package to this metadata instance
		PackageInfo* AddMaterialPackage(void);

		//! Add a file package to this metadata instance with a single essence track for the specified essence
		PackageInfo* AddFilePackage(UInt32 BodySID, EssenceSourcePtr &Source, WrappingConfigPtr &Config);

		//! Add an empty file package to this metadata instance
		PackageInfo* AddFilePackage(void);

		//! Add a timecode track to the specified package in this instance
		TimecodeTrackInfo* AddTimecodeTrack(int PackageID);

		//! Set the duration of every essence and timecode track to a given value
		void SetDurations(Length Duration);

		//! Auto set the duration of every essence and timecode track in this instance
		void SetDurations(void);

		//! Update the existing instance data
		bool UpdateInstance(void);

		//! Get a pointer to the first material package that was added when the instance was constructed
		PackageInfo* MaterialPackage(void) { return pMaterialPackage; }

		//! Get a poitner to the file package that was added when the instance was constructed, zero if no file package was built during construction
		PackageInfo* FilePackage(void) { return pFilePackage; }

		//! Get a pointer to the actual Metadata instance that this info block describes
		MetadataPtr &GetInstance(void)
		{
			if(!Instance) BuildInstance();
			return Instance;
		}


		//! Build a metadata info block from an existing Metadata instance
		static MetadataInfoPtr Parse(MetaBuilder *Parent, Metadata *Instance);

	protected:
		//! Build an instance of the metadata for this info block
		bool BuildInstance(void);

		//! Determine and set the Operational Pattern label for this metadata instance
		void SetOPLabel(void);

		// Allow our holder to call protected build commands
		friend class MetaBuilder;
	};



	//! Structure for holding the version number of products
	/*! DRAGONS: The format of this structure is as per 377-1-2009, not 377-2004 */
	struct ProductVersionStruct
	{
		UInt16	Major;
		UInt16	Minor;
		UInt16	Tertiary;
		UInt16	Patch;
		UInt16	Release;

		ProductVersionStruct(UInt16 Major, UInt16 Minor, UInt16 Tertiary, UInt16 Patch, UInt16 Release)
			: Major(Major), Minor(Minor), Tertiary(Tertiary), Patch(Patch), Release(Release) {}

		bool IsEmpty(void)
		{
			// Return true if all zero
			return (Major == 0) && (Minor == 0) && (Tertiary == 0) && (Patch == 0) && (Release == 0);
		}

		std::string GetString(bool CommaSeparated = false)
		{
			std::stringstream s;
			if(CommaSeparated)
				s << Major << ',' << Minor << ',' << Tertiary << ',' << Patch << ',' << Release;
			else
			{			
				s << Major << '.' << Minor << '.' << Tertiary << '(' << Patch << ')';
				switch(Release)
				{
					case 1: s << "-Release"; break;
					case 2: s << "-Development"; break;
					case 3: s << "-Development"; break;
					case 4: s << "-Development"; break;
					case 5: s << "-Development"; break;
					default: s << '-' << Release; break;
				}
			}

			return s.str();
		}
	};



	//! Class that manages the building of complete header metadata instances, individual or linked groups
	class MetaBuilder : public RefCount<MetaBuilder>
	{
	protected:
		MetadataInfoList Instances;				//!< List of metadata instances that we will build or have built
		MetadataInfoPtr Main;					//!< The main instance, if NULL then the first entry in Instances is regarded as the main one

		// Details for Identification sets
		std::string CompanyName;				//!< The company name for the ident set
		std::string ProductName;				//!< The product name for the ident set
		std::string VersionString;				//!< The product version string for the ident set
		ProductVersionStruct VersionStruct;		//!< The product version structure for the ident set, all values 0 if not set
		mxflib::UUID ProductUID;				//!< The product UID for the ident set

	public:
		MetaBuilder() 
			: CompanyName("Company"), ProductName("Product"), VersionString("Version"), VersionStruct(0,0,0,0,0) {}

		//! Add a new metadata instance, without adding any know sources
		MetadataInfo* AddInstance(void);

		//! Add a new metadata instance, with a specified internal source
		MetadataInfo* AddInstance(UInt32 BodySID, EssenceSourcePtr &Source, WrappingConfigPtr &Config);

		//! Update all existing instance
		void UpdateAllInstances(void)
		{
			MetadataInfoList::iterator it = Instances.begin();
			while(it != Instances.end())
			{
				(*it)->UpdateInstance();
				it++;
			}
		}

		//! Set the details that will be used for building Identification sets
		void SetIdent(std::string CompanyName, std::string ProductName, std::string ProductVersion, UUIDPtr &ProductUID)
		{
			SetIdent(CompanyName, ProductName, ProductVersion, ProductVersionStruct(0,0,0,0,0), ProductUID);
		}

		//! Set the details that will be used for building Identification sets
		void SetIdent(std::string CompanyName, std::string ProductName, std::string VersionString, ProductVersionStruct VersionStruct, UUIDPtr &ProductUID);

		//! Auto set the duration of every essence and timecode track in the given instance
		void SetDurations(int InstanceID = -1);

		//! Set the duration of every essence and timecode track in the master instance to a given value
		void SetDurations(Length Duration) { SetDurations(-1, Duration); }

		//! Set the duration of every essence and timecode track in a given instance to a given value
		void SetDurations(int InstanceID, Length Duration);

		// Allow held meta instance info blocks to acces our internals
		friend class MetadataInfo;
	};


	//! Smart pointer to a MetaBuilder
	typedef SmartPtr<MetaBuilder> MetaBuilderPtr;
}

#endif // MXFLIB__METABUILDER_H
