	// Class definitions converted from file DMS_AS_03.xml
	MXFLIB_TYPE_START(AS03_Labels)
			MXFLIB_LABEL("DMS_AS_03_Core", "Metadata for AS_03 Delivery Format", "urn:x-ul:060e2b34.04010101.0d010701.03010000")
			MXFLIB_LABEL("DMS_AS_03_PBS", "Metadata for AS_03 PBS Delivery Format", "urn:x-ul:060e2b34.04010101.0d010401.03000000")
			MXFLIB_LABEL("DMS_AS_12", "Metadata for AS_12 Commercial Distribution Format", "urn:smpte:ul:060e2b34.04010101.0d010701.0c010000")
	MXFLIB_TYPE_END

	// Class definitions converted from file DMS_AS_03_Core.xml
	MXFLIB_CLASS_START(DMS_AS_03_Core_Classes)
		MXFLIB_CLASS_SET("DMS_AS_03_Core_Framework", "AS_03 Descriptive Metadata", "DM_Framework", "urn:x-ul:060e2b34.02530101.0d010701.03010100")
			MXFLIB_CLASS_ITEM("AS_03_Identifier", "the program identifier as a text string, e.g. P900000HD", ClassUsageRequired, "UTF16String", 0, 0, 0x0000, "urn:x-ul:060e2b34.01010101.0d010401.03010200", NULL, NULL)
			MXFLIB_CLASS_ITEM("AS_03_IdentifierKind", "a controlled-vocabulary string value indicating the kind of program identifier, e.g. PBS PackageID", ClassUsageRequired, "UTF16String", 0, 0, 0x0000, "urn:x-ul:060e2b34.01010101.0d010401.03010100", NULL, NULL)
			MXFLIB_CLASS_ITEM("AS_03_ShimName", "a controlled-vocabulary string value indicating the AS-03 Shim Name, e.g. PBS NGIS HD Playout", ClassUsageRequired, "UTF16String", 0, 0, 0x0000, "urn:x-ul:060e2b34.01010101.0d010401.03010300", NULL, NULL)
			MXFLIB_CLASS_ITEM("AS_03_SignalStandard", "a controlled-vocabulary string value indicating the signal standard of video contained in this AS-03 file", ClassUsageRequired, "UTF16String", 0, 0, 0x0000, "urn:x-ul:060e2b34.01010101.0d010401.03010400", NULL, NULL)
			MXFLIB_CLASS_ITEM("AS_03_IntendedAFD", "a string value indicating the intended display format for the program, per SMPTE 2016-1 table 1 a3 a2 a1 a0 with optional informative appended text e.g. 1001 Pillarbox, 0100 Letterbox, 1000 FullHD", ClassUsageRequired, "UTF16String", 0, 0, 0x0000, "urn:x-ul:060e2b34.01010101.0d010401.03010500", NULL, NULL)
		MXFLIB_CLASS_SET_END
	MXFLIB_CLASS_END

	// Class definitions converted from file DMS_AS_03_PBS.xml
	MXFLIB_CLASS_START(DMS_AS_03_PBS_Classes)
		MXFLIB_CLASS_SET("DMS_AS_03_PBS_Framework", "AS_03 PBS Descriptive Metadata", "urn:x-ul:060e2b34.02530101.0d010701.03010100", "urn:x-ul:060e2b34.02530101.0d010401.03010000")
			MXFLIB_CLASS_ITEM("AS_03_SlateTitle", "a string that specifies a program title to be displayed to traffic and master control operators, e.g. Sesame Street", ClassUsageOptional, "UTF16String", 0, 0, 0x0000, "urn:x-ul:060e2b34.01010101.0d010401.03010600", NULL, NULL)
			MXFLIB_CLASS_ITEM("AS_03_NOLACode", "a string that specifies the program series code and episode number, e.g. SESA 4187", ClassUsageOptional, "UTF16String", 0, 0, 0x0000, "urn:x-ul:060e2b34.01010101.0d010401.03010700", NULL, NULL)
			MXFLIB_CLASS_ITEM("AS_03_Rating", "a controlled-vocabulary string that specifies the V-Chip rating of the program, e.g. TV-G", ClassUsageOptional, "UTF16String", 0, 0, 0x0000, "urn:x-ul:060e2b34.01010101.0d010401.03010800", NULL, NULL)
			MXFLIB_CLASS_ITEM("AS_03_NielsenStreamIdentifier", "a string that specifies Nielsen stream identifier for the program", ClassUsageOptional, "UTF16String", 0, 0, 0x0000, "urn:x-ul:060e2b34.01010101.0d010401.03010900", NULL, NULL)
		MXFLIB_CLASS_SET_END
	MXFLIB_CLASS_END

	// Class definitions converted from file DMS_AS_12.xml
	MXFLIB_CLASS_START(DMS_AS_12_Classes)
		MXFLIB_CLASS_EXTEND("DMS_AS_12_Framework", "AS_12 Descriptive Metadata", "urn:smpte:ul:060e2b34.02530101.0d010401.00000000", "urn:smpte:ul:060e2b34.02530101.0d010701.0c010100")
			MXFLIB_CLASS_ITEM("AS_12_ShimName", "a controlled-vocabulary string value indicating the AS-12 Shim Name, e.g. Ad-ID Digital Identification", ClassUsageRequired, "UTF16String", 0, 0, 0x0000, "urn:smpte:ul:060e2b34.01010101.0d010701.0c010101", NULL, NULL)
			MXFLIB_CLASS_ITEM("AS_12_Slate", "", ClassUsageRequired, "StrongRef", 0, 0, 0x0000, "urn:smpte:ul:060e2b34.01010101.0d010701.0c010102", NULL, NULL)
		MXFLIB_CLASS_EXTEND_END
		MXFLIB_CLASS_SET("AS_12_DescriptiveObject", "AS_12 Commercial Distribution Slate Metadata", "urn:smpte:ul:060e2b34.02530101.0d010400.00000000", "urn:smpte:ul:060e2b34.02530101.0d010701.0c010100")
		MXFLIB_CLASS_SET_END
		MXFLIB_CLASS_EXTEND("DMS_AS_12_AdID_Slate", "Ad-ID Slate Metadata", "urn:smpte:ul:060e2b34.02530101.0d010701.0c010100", "urn:smpte:ul:060E2B34.0253010d.0d0d0100.00000000")
			MXFLIB_CLASS_ITEM("adid_prefix", "The four character prefix advertising asset encoded in the file.", ClassUsageRequired, "UTF16String", 0, 0, 0x0000, "urn:smpte:ul:060E2B34.0101010d.0d0d0101.00000000", NULL, NULL)
			MXFLIB_CLASS_ITEM("adid_code", "The unique identifier for the advertising asset encoded in the file.", ClassUsageRequired, "UTF16String", 0, 0, 0x0000, "urn:smpte:ul:060E2B34.0101010d.0d0d0102.00000000", NULL, NULL)
			MXFLIB_CLASS_ITEM("ad_title", "Alpha-numeric combination that follows the prefix. (128 character string)", ClassUsageRequired, "UTF16String", 0, 0, 0x0000, "urn:smpte:ul:060E2B34.0101010d.0d0d0103.00000000", NULL, NULL)
			MXFLIB_CLASS_ITEM("brand", "The advertiser brand that is associated to this product. E.g. Joe's Bakery (64 character string)", ClassUsageRequired, "UTF16String", 0, 0, 0x0000, "urn:smpte:ul:060E2B34.0101010d.0d0d0104.00000000", NULL, NULL)
			MXFLIB_CLASS_ITEM("product", "The product that is the extension of the brand. e.g. Baked Goods (32 character string)", ClassUsageRequired, "UTF16String", 0, 0, 0x0000, "urn:smpte:ul:060E2B34.0101010d.0d0d0105.00000000", NULL, NULL)
			MXFLIB_CLASS_ITEM("advertiser", "The company or the agency's client that is advertising. e.g. Joe's Bakery (64 character string)", ClassUsageRequired, "UTF16String", 0, 0, 0x0000, "urn:smpte:ul:060E2B34.0101010d.0d0d0106.00000000", NULL, NULL)
			MXFLIB_CLASS_ITEM("agency_office_location", "The agency working with the advertiser and their office location. (64 character string)", ClassUsageRequired, "UTF16String", 0, 0, 0x0000, "urn:smpte:ul:060E2B34.0101010d.0d0d0107.00000000", NULL, NULL)
			MXFLIB_CLASS_ITEM("length", "The intended length of the advertisement between SOM and EOM", ClassUsageRequired, "UTF16String", 0, 0, 0x0000, "urn:smpte:ul:060E2B34.0101010d.0d0d0108.00000000", NULL, NULL)
		MXFLIB_CLASS_EXTEND_END
	MXFLIB_CLASS_END

	// Build a complete dictionary from above types and classes
	MXFLIB_DICTIONARY_START(AS03_DictData)
	MXFLIB_DICTIONARY_TYPES(AS03_Labels)
	MXFLIB_DICTIONARY_CLASSES(DMS_AS_03_Core_Classes)
	MXFLIB_DICTIONARY_CLASSES(DMS_AS_03_PBS_Classes)
	MXFLIB_DICTIONARY_CLASSES(DMS_AS_12_Classes)
	MXFLIB_DICTIONARY_END


	// Define ULs for the global keys in this dictionary
	namespace mxflib
	{
		const UInt8 AS_03_Identifier_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x04, 0x01, 0x03, 0x01, 0x02, 0x00 };
		const UL AS_03_Identifier_UL(AS_03_Identifier_UL_Data);

		const UInt8 AS_03_IdentifierKind_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x04, 0x01, 0x03, 0x01, 0x01, 0x00 };
		const UL AS_03_IdentifierKind_UL(AS_03_IdentifierKind_UL_Data);

		const UInt8 AS_03_IntendedAFD_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x04, 0x01, 0x03, 0x01, 0x05, 0x00 };
		const UL AS_03_IntendedAFD_UL(AS_03_IntendedAFD_UL_Data);

		const UInt8 AS_03_ShimName_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x04, 0x01, 0x03, 0x01, 0x03, 0x00 };
		const UL AS_03_ShimName_UL(AS_03_ShimName_UL_Data);

		const UInt8 AS_03_SignalStandard_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x04, 0x01, 0x03, 0x01, 0x04, 0x00 };
		const UL AS_03_SignalStandard_UL(AS_03_SignalStandard_UL_Data);

		const UInt8 DMS_AS_03_Core_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x04, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x07, 0x01, 0x03, 0x01, 0x00, 0x00 };
		const UL DMS_AS_03_Core_UL(DMS_AS_03_Core_UL_Data);

		const UInt8 DMS_AS_03_Core_Framework_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x02, 0x53, 0x01, 0x01, 0x0d, 0x01, 0x07, 0x01, 0x03, 0x01, 0x01, 0x00 };
		const UL DMS_AS_03_Core_Framework_UL(DMS_AS_03_Core_Framework_UL_Data);

	} // namespace mxflib

	// Define ULs for the global keys in this dictionary
	namespace mxflib
	{
		const UInt8 AS_03_NOLACode_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x04, 0x01, 0x03, 0x01, 0x07, 0x00 };
		const UL AS_03_NOLACode_UL(AS_03_NOLACode_UL_Data);

		const UInt8 AS_03_NielsenStreamIdentifier_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x04, 0x01, 0x03, 0x01, 0x09, 0x00 };
		const UL AS_03_NielsenStreamIdentifier_UL(AS_03_NielsenStreamIdentifier_UL_Data);

		const UInt8 AS_03_Rating_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x04, 0x01, 0x03, 0x01, 0x08, 0x00 };
		const UL AS_03_Rating_UL(AS_03_Rating_UL_Data);

		const UInt8 AS_03_SlateTitle_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x04, 0x01, 0x03, 0x01, 0x06, 0x00 };
		const UL AS_03_SlateTitle_UL(AS_03_SlateTitle_UL_Data);

		const UInt8 DMS_AS_03_PBS_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x04, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x04, 0x01, 0x03, 0x00, 0x00, 0x00 };
		const UL DMS_AS_03_PBS_UL(DMS_AS_03_PBS_UL_Data);

		const UInt8 DMS_AS_03_PBS_Framework_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x02, 0x53, 0x01, 0x01, 0x0d, 0x01, 0x04, 0x01, 0x03, 0x01, 0x00, 0x00 };
		const UL DMS_AS_03_PBS_Framework_UL(DMS_AS_03_PBS_Framework_UL_Data);

	} // namespace mxflib

	// Define ULs for the global keys in this dictionary
	namespace mxflib
	{
		const UInt8 AS_12_ShimName_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x07, 0x01, 0x0c, 0x01, 0x01, 0x01 };
		const UL AS_12_ShimName_UL(AS_12_ShimName_UL_Data);

		const UInt8 AS_12_Slate_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x07, 0x01, 0x0c, 0x01, 0x01, 0x02 };
		const UL AS_12_Slate_UL(AS_12_Slate_UL_Data);

		const UInt8 DMS_AS_12_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x04, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x07, 0x01, 0x0c, 0x01, 0x00, 0x00 };
		const UL DMS_AS_12_UL(DMS_AS_12_UL_Data);

		const UInt8 DMS_AS_12_AdID_Slate_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x02, 0x53, 0x01, 0x0d, 0x0d, 0x0d, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00 };
		const UL DMS_AS_12_AdID_Slate_UL(DMS_AS_12_AdID_Slate_UL_Data);

		const UInt8 DMS_AS_12_Framework_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x02, 0x53, 0x01, 0x01, 0x0d, 0x01, 0x07, 0x01, 0x0c, 0x01, 0x01, 0x00 };
		const UL DMS_AS_12_Framework_UL(DMS_AS_12_Framework_UL_Data);

		const UInt8 AS_12_DescriptiveObject_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x02, 0x53, 0x01, 0x01, 0x0d, 0x01, 0x07, 0x01, 0x0c, 0x01, 0x02, 0x00 };
		const UL AS_12_DescriptiveObject_UL(AS_12_DescriptiveObject_UL_Data);

		const UInt8 ad_title_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x0d, 0x0d, 0x0d, 0x01, 0x03, 0x00, 0x00, 0x00, 0x00 };
		const UL ad_title_UL(ad_title_UL_Data);

		const UInt8 adid_code_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x0d, 0x0d, 0x0d, 0x01, 0x02, 0x00, 0x00, 0x00, 0x00 };
		const UL adid_code_UL(adid_code_UL_Data);

		const UInt8 adid_prefix_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x0d, 0x0d, 0x0d, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00 };
		const UL adid_prefix_UL(adid_prefix_UL_Data);

		const UInt8 advertiser_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x0d, 0x0d, 0x0d, 0x01, 0x06, 0x00, 0x00, 0x00, 0x00 };
		const UL advertiser_UL(advertiser_UL_Data);

		const UInt8 agency_office_location_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x0d, 0x0d, 0x0d, 0x01, 0x07, 0x00, 0x00, 0x00, 0x00 };
		const UL agency_office_location_UL(agency_office_location_UL_Data);

		const UInt8 brand_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x0d, 0x0d, 0x0d, 0x01, 0x04, 0x00, 0x00, 0x00, 0x00 };
		const UL brand_UL(brand_UL_Data);

		const UInt8 length_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x0d, 0x0d, 0x0d, 0x01, 0x08, 0x00, 0x00, 0x00, 0x00 };
		const UL length_UL(length_UL_Data);

		const UInt8 product_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x0d, 0x0d, 0x0d, 0x01, 0x05, 0x00, 0x00, 0x00, 0x00 };
		const UL product_UL(product_UL_Data);

	} // namespace mxflib
