
	// Label definitions converted from file C:\dev\mxflib\dict_as11.xml
	MXFLIB_TYPE_START(AS11_Types)
		MXFLIB_LABEL("AS11CoreSchemeLabel", "AS-11 Core DM Scheme", "06 0e 2b 34 04 01 01 01 0d 01 07 01 0b 01 00 00")
		MXFLIB_LABEL("AS11SegmentationSchemeLabel", "AS-11 Segmentation DM Scheme", "06 0e 2b 34 04 01 01 01 0d 01 07 01 0b 02 00 00")
		MXFLIB_LABEL("UKDPPSchemeLabel", "UK DPP DM Scheme", "06 0e 2b 34 04 01 01 01 0d 0c 01 01 01 00 00 00")
	MXFLIB_TYPE_END

	// Types definitions converted from file C:\dev\mxflib\dict_as11.xml
	MXFLIB_TYPE_START(AS11_Types_2)
		MXFLIB_TYPE_ENUM("CaptionsType", "", "UInt8", "")
			MXFLIB_TYPE_ENUM_VALUE("HardOfHearing", "", "0")
			MXFLIB_TYPE_ENUM_VALUE("Translation", "", "1")
		MXFLIB_TYPE_ENUM_END
		MXFLIB_TYPE_ENUM("AudioTrackLayoutType", "", "UInt8", "")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R48_1a", "", "0")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R48_1b", "", "1")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R48_1c", "", "2")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R48_2a", "", "3")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R48_2b", "", "4")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R48_2c", "", "5")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R48_3a", "", "6")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R48_3b", "", "7")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R48_4a", "", "8")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R48_4b", "", "9")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R48_4c", "", "10")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R48_5a", "", "11")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R48_5b", "", "12")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R48_6a", "", "13")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R48_6b", "", "14")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R48_7a", "", "15")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R48_7b", "", "16")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R48_8a", "", "17")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R48_8b", "", "18")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R48_8c", "", "19")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R48_9a", "", "20")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R48_9b", "", "21")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R48_10a", "", "22")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R48_11a", "", "23")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R48_11b", "", "24")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R48_11c", "", "25")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R123_2a", "", "26")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R123_4a", "", "27")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R123_4b", "", "28")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R123_4c", "", "29")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R123_8a", "", "30")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R123_8b", "", "31")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R123_8c", "", "32")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R123_8d", "", "33")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R123_8e", "", "34")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R123_8f", "", "35")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R123_8g", "", "36")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R123_8h", "", "37")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R123_8i", "", "38")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R123_12a", "", "39")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R123_12b", "", "40")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R123_12c", "", "41")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R123_12d", "", "42")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R123_12e", "", "43")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R123_12f", "", "44")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R123_12g", "", "45")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R123_12h", "", "46")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R123_16a", "", "47")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R123_16b", "", "48")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R123_16c", "", "49")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R123_16d", "", "50")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R123_16e", "", "51")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R123_16f", "", "52")
			MXFLIB_TYPE_ENUM_VALUE("Undefined", "", "255")
		MXFLIB_TYPE_ENUM_END
		MXFLIB_TYPE_ENUM("FPAPassType", "", "UInt8", "")
			MXFLIB_TYPE_ENUM_VALUE("Yes", "", "0")
			MXFLIB_TYPE_ENUM_VALUE("No", "", "1")
			MXFLIB_TYPE_ENUM_VALUE("NotTested", "", "2")
		MXFLIB_TYPE_ENUM_END
		MXFLIB_TYPE_ENUM("SigningPresentType", "", "UInt8", "")
			MXFLIB_TYPE_ENUM_VALUE("Yes", "", "0")
			MXFLIB_TYPE_ENUM_VALUE("No", "", "1")
			MXFLIB_TYPE_ENUM_VALUE("SignerOnly", "", "2")
		MXFLIB_TYPE_ENUM_END
		MXFLIB_TYPE_ENUM("ThreeDTypeType", "", "UInt8", "")
			MXFLIB_TYPE_ENUM_VALUE("SideBySide", "", "0")
			MXFLIB_TYPE_ENUM_VALUE("Dual", "", "1")
			MXFLIB_TYPE_ENUM_VALUE("LeftEyeOnly", "", "2")
			MXFLIB_TYPE_ENUM_VALUE("RightEyeOnly", "", "3")
		MXFLIB_TYPE_ENUM_END
		MXFLIB_TYPE_ENUM("AudioLoudStdType", "", "UInt8", "")
			MXFLIB_TYPE_ENUM_VALUE("None", "", "0")
			MXFLIB_TYPE_ENUM_VALUE("EBU_R128", "", "1")
		MXFLIB_TYPE_ENUM_END
		MXFLIB_TYPE_ENUM("AudioDescType", "", "UInt8", "")
			MXFLIB_TYPE_ENUM_VALUE("ControlData_Narration", "", "0")
			MXFLIB_TYPE_ENUM_VALUE("ADMix", "", "1")
		MXFLIB_TYPE_ENUM_END
		MXFLIB_TYPE_ENUM("SignLanguageType", "", "UInt8", "")
			MXFLIB_TYPE_ENUM_VALUE("BritishSignLanguage", "", "0")
			MXFLIB_TYPE_ENUM_VALUE("Makaton", "", "1")
		MXFLIB_TYPE_ENUM_END
	MXFLIB_TYPE_END

	// Class definitions converted from file C:\dev\mxflib\dict_as11.xml
	MXFLIB_CLASS_START(AS11_Classes)
		MXFLIB_CLASS_SET("AS11CoreFramework", "", "DM_Framework", "06 0e 2b 34 02 53 01 01 0d 01 07 01 0b 01 01 00")
			MXFLIB_CLASS_ITEM("SeriesTitle", "", ClassUsageRequired, "UTF16String", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 01 07 01 0b 01 01 01", NULL, NULL)
			MXFLIB_CLASS_ITEM("ProgrammeTitle", "", ClassUsageRequired, "UTF16String", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 01 07 01 0b 01 01 02", NULL, NULL)
			MXFLIB_CLASS_ITEM("EpisodeTitleNumber", "", ClassUsageRequired, "UTF16String", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 01 07 01 0b 01 01 03", NULL, NULL)
			MXFLIB_CLASS_ITEM("ShimName", "", ClassUsageRequired, "UTF16String", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 01 07 01 0b 01 01 04", NULL, NULL)
			MXFLIB_CLASS_ITEM("AudioTrackLayout", "", ClassUsageRequired, "AudioTrackLayoutType", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 01 07 01 0b 01 01 05", NULL, NULL)
			MXFLIB_CLASS_ITEM("AFD", "", ClassUsageRequired, "UInt8", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 01 04 01 0b 01 01 05", NULL, NULL)
			MXFLIB_CLASS_ITEM("PrimaryAudioLanguage", "", ClassUsageRequired, "UTF16String", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 01 07 01 0b 01 01 06", NULL, NULL)
			MXFLIB_CLASS_ITEM("ClosedCaptionsPresent", "", ClassUsageRequired, "Boolean", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 01 07 01 0b 01 01 07", NULL, NULL)
			MXFLIB_CLASS_ITEM("ClosedCaptionsType", "", ClassUsageOptional, "CaptionsType", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 01 07 01 0b 01 01 08", NULL, NULL)
			MXFLIB_CLASS_ITEM("ClosedCaptionsLanguage", "", ClassUsageOptional, "UTF16String", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 01 07 01 0b 01 01 09", NULL, NULL)
		MXFLIB_CLASS_SET_END
		MXFLIB_CLASS_SET("DMSegmentationFramework", "", "DM_Framework", "06 0e 2b 34 02 53 01 01 0d 01 07 01 01 01 01 00")
		MXFLIB_CLASS_SET_END
		MXFLIB_CLASS_SET("AS11SegmentationFramework", "", "DMSegmentationFramework", "06 0e 2b 34 02 53 01 01 0d 01 07 01 0b 02 01 00")
			MXFLIB_CLASS_ITEM("PartNumber", "", ClassUsageRequired, "UInt16", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 01 07 01 0b 02 01 01", NULL, NULL)
			MXFLIB_CLASS_ITEM("PartTotal", "", ClassUsageRequired, "UInt16", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 01 07 01 0b 02 01 02", NULL, NULL)
		MXFLIB_CLASS_SET_END
		MXFLIB_CLASS_SET("UKDPPFramework", "", "DM_Framework", "06 0e 2b 34 02 53 01 01 0d 0c 01 01 01 01 00 00")
			MXFLIB_CLASS_ITEM("ProductionNumber", "", ClassUsageRequired, "UTF16String", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 01 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("Synopsis", "", ClassUsageRequired, "UTF16String", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 02 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("Originator", "", ClassUsageRequired, "UTF16String", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 03 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("CopyrightYear", "", ClassUsageRequired, "UInt16", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 04 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("OtherIdentifier", "", ClassUsageOptional, "UTF16String", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 05 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("OtherIdentifierType", "", ClassUsageOptional, "UTF16String", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 06 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("Genre", "", ClassUsageOptional, "UTF16String", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 07 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("Distributor", "", ClassUsageOptional, "UTF16String", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 08 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("PictureRatio", "", ClassUsageOptional, "Rational", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 09 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("ThreeD", "", ClassUsageRequired, "Boolean", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 0a 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("ThreeDType", "", ClassUsageOptional, "ThreeDTypeType", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 0b 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("ProductPlacement", "", ClassUsageOptional, "Boolean", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 0c 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("FPAPass", "", ClassUsageOptional, "FPAPassType", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 0d 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("FPAManufacturer", "", ClassUsageOptional, "UTF16String", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 0e 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("FPAVersion", "", ClassUsageOptional, "UTF16String", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 0f 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("VideoComments", "", ClassUsageOptional, "UTF16String", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 10 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("SecondaryAudioLanguage", "", ClassUsageRequired, "UTF16String", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 11 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("TertiaryAudioLanguage", "", ClassUsageRequired, "UTF16String", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 12 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("AudioLoudnessStandard", "", ClassUsageRequired, "AudioLoudStdType", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 13 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("AudioComments", "", ClassUsageOptional, "UTF16String", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 14 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("LineUpStart", "", ClassUsageRequired, "Position", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 15 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("IdentClockStart", "", ClassUsageRequired, "Position", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 16 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("TotalNumberOfParts", "", ClassUsageRequired, "UInt16", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 17 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("TotalProgrammeDuration", "", ClassUsageRequired, "LengthType", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 18 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("AudioDescriptionPresent", "", ClassUsageRequired, "Boolean", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 19 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("AudioDescriptionType", "", ClassUsageOptional, "AudioDescType", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 1a 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("OpenCaptionsPresent", "", ClassUsageRequired, "Boolean", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 1b 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("OpenCaptionsType", "", ClassUsageOptional, "CaptionsType", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 1c 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("OpenCaptionsLanguage", "", ClassUsageOptional, "UTF16String", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 1d 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("SigningPresent", "", ClassUsageRequired, "SigningPresentType", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 1e 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("SignLanguage", "", ClassUsageOptional, "SignLanguageType", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 1f 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("CompletionDate", "", ClassUsageRequired, "Timestamp", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 20 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("TextlessElementsExist", "", ClassUsageOptional, "Boolean", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 21 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("ProgrammeHasText", "", ClassUsageOptional, "Boolean", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 22 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("ProgrammeTextLanguage", "", ClassUsageOptional, "UTF16String", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 23 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("ContactEmail", "", ClassUsageRequired, "UTF16String", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 24 00", NULL, NULL)
			MXFLIB_CLASS_ITEM("ContactTelephoneNo", "", ClassUsageRequired, "UTF16String", 0, 0, 0x0000, "06 0e 2b 34 01 01 01 01 0d 0c 01 01 01 01 25 00", NULL, NULL)
		MXFLIB_CLASS_SET_END
	MXFLIB_CLASS_END

	// Build a complete dictionary from above types and classes
	MXFLIB_DICTIONARY_START(AS11)
		MXFLIB_DICTIONARY_TYPES(AS11_Types)
		MXFLIB_DICTIONARY_TYPES(AS11_Types_2)
		MXFLIB_DICTIONARY_CLASSES(AS11_Classes)
	MXFLIB_DICTIONARY_END


	// Define ULs for the global keys in this dictionary
	namespace AS_11
	{
		const UInt8 AS11CoreFramework_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x02, 0x53, 0x01, 0x01, 0x0d, 0x01, 0x07, 0x01, 0x0b, 0x01, 0x01, 0x00 };
		const UL AS11CoreFramework_UL(AS11CoreFramework_UL_Data);

		const UInt8 AS11CoreSchemeLabel_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x04, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x07, 0x01, 0x0b, 0x01, 0x00, 0x00 };
		const UL AS11CoreSchemeLabel_UL(AS11CoreSchemeLabel_UL_Data);

		const UInt8 AS11SegmentationFramework_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x02, 0x53, 0x01, 0x01, 0x0d, 0x01, 0x07, 0x01, 0x0b, 0x02, 0x01, 0x00 };
		const UL AS11SegmentationFramework_UL(AS11SegmentationFramework_UL_Data);

		const UInt8 AS11SegmentationSchemeLabel_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x04, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x07, 0x01, 0x0b, 0x02, 0x00, 0x00 };
		const UL AS11SegmentationSchemeLabel_UL(AS11SegmentationSchemeLabel_UL_Data);

		const UInt8 AudioComments_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x14, 0x00 };
		const UL AudioComments_UL(AudioComments_UL_Data);

		const UInt8 AudioDescriptionPresent_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x19, 0x00 };
		const UL AudioDescriptionPresent_UL(AudioDescriptionPresent_UL_Data);

		const UInt8 AudioDescriptionType_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x1a, 0x00 };
		const UL AudioDescriptionType_UL(AudioDescriptionType_UL_Data);

		const UInt8 AudioLoudnessStandard_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x13, 0x00 };
		const UL AudioLoudnessStandard_UL(AudioLoudnessStandard_UL_Data);

		const UInt8 AudioTrackLayout_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x07, 0x01, 0x0b, 0x01, 0x01, 0x05 };
		const UL AudioTrackLayout_UL(AudioTrackLayout_UL_Data);

		const UInt8 AFD_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x05, 0x01, 0x0b, 0x01, 0x01, 0x05 };
		const UL AFD_UL(AFD_UL_Data);

		const UInt8 ClosedCaptionsLanguage_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x07, 0x01, 0x0b, 0x01, 0x01, 0x09 };
		const UL ClosedCaptionsLanguage_UL(ClosedCaptionsLanguage_UL_Data);

		const UInt8 ClosedCaptionsPresent_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x07, 0x01, 0x0b, 0x01, 0x01, 0x07 };
		const UL ClosedCaptionsPresent_UL(ClosedCaptionsPresent_UL_Data);

		const UInt8 ClosedCaptionsType_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x07, 0x01, 0x0b, 0x01, 0x01, 0x08 };
		const UL ClosedCaptionsType_UL(ClosedCaptionsType_UL_Data);

		const UInt8 CompletionDate_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x20, 0x00 };
		const UL CompletionDate_UL(CompletionDate_UL_Data);

		const UInt8 ContactEmail_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x24, 0x00 };
		const UL ContactEmail_UL(ContactEmail_UL_Data);

		const UInt8 ContactTelephoneNo_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x25, 0x00 };
		const UL ContactTelephoneNo_UL(ContactTelephoneNo_UL_Data);

		const UInt8 CopyrightYear_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x04, 0x00 };
		const UL CopyrightYear_UL(CopyrightYear_UL_Data);

		const UInt8 DMSegmentationFramework_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x02, 0x53, 0x01, 0x01, 0x0d, 0x01, 0x07, 0x01, 0x01, 0x01, 0x01, 0x00 };
		const UL DMSegmentationFramework_UL(DMSegmentationFramework_UL_Data);

		const UInt8 Distributor_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x08, 0x00 };
		const UL Distributor_UL(Distributor_UL_Data);

		const UInt8 EpisodeTitleNumber_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x07, 0x01, 0x0b, 0x01, 0x01, 0x03 };
		const UL EpisodeTitleNumber_UL(EpisodeTitleNumber_UL_Data);

		const UInt8 FPAManufacturer_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x0e, 0x00 };
		const UL FPAManufacturer_UL(FPAManufacturer_UL_Data);

		const UInt8 FPAPass_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x00 };
		const UL FPAPass_UL(FPAPass_UL_Data);

		const UInt8 FPAVersion_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x0f, 0x00 };
		const UL FPAVersion_UL(FPAVersion_UL_Data);

		const UInt8 Genre_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x07, 0x00 };
		const UL Genre_UL(Genre_UL_Data);

		const UInt8 IdentClockStart_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x16, 0x00 };
		const UL IdentClockStart_UL(IdentClockStart_UL_Data);

		const UInt8 LineUpStart_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x15, 0x00 };
		const UL LineUpStart_UL(LineUpStart_UL_Data);

		const UInt8 OpenCaptionsLanguage_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x1d, 0x00 };
		const UL OpenCaptionsLanguage_UL(OpenCaptionsLanguage_UL_Data);

		const UInt8 OpenCaptionsPresent_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x1b, 0x00 };
		const UL OpenCaptionsPresent_UL(OpenCaptionsPresent_UL_Data);

		const UInt8 OpenCaptionsType_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x1c, 0x00 };
		const UL OpenCaptionsType_UL(OpenCaptionsType_UL_Data);

		const UInt8 Originator_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x03, 0x00 };
		const UL Originator_UL(Originator_UL_Data);

		const UInt8 OtherIdentifier_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x05, 0x00 };
		const UL OtherIdentifier_UL(OtherIdentifier_UL_Data);

		const UInt8 OtherIdentifierType_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x06, 0x00 };
		const UL OtherIdentifierType_UL(OtherIdentifierType_UL_Data);

		const UInt8 PartNumber_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x07, 0x01, 0x0b, 0x02, 0x01, 0x01 };
		const UL PartNumber_UL(PartNumber_UL_Data);

		const UInt8 PartTotal_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x07, 0x01, 0x0b, 0x02, 0x01, 0x02 };
		const UL PartTotal_UL(PartTotal_UL_Data);

		const UInt8 PictureRatio_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x09, 0x00 };
		const UL PictureRatio_UL(PictureRatio_UL_Data);

		const UInt8 PrimaryAudioLanguage_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x07, 0x01, 0x0b, 0x01, 0x01, 0x06 };
		const UL PrimaryAudioLanguage_UL(PrimaryAudioLanguage_UL_Data);

		const UInt8 ProductPlacement_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x0c, 0x00 };
		const UL ProductPlacement_UL(ProductPlacement_UL_Data);

		const UInt8 ProductionNumber_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x01, 0x00 };
		const UL ProductionNumber_UL(ProductionNumber_UL_Data);

		const UInt8 ProgrammeHasText_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x22, 0x00 };
		const UL ProgrammeHasText_UL(ProgrammeHasText_UL_Data);

		const UInt8 ProgrammeTextLanguage_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x23, 0x00 };
		const UL ProgrammeTextLanguage_UL(ProgrammeTextLanguage_UL_Data);

		const UInt8 ProgrammeTitle_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x07, 0x01, 0x0b, 0x01, 0x01, 0x02 };
		const UL ProgrammeTitle_UL(ProgrammeTitle_UL_Data);

		const UInt8 SecondaryAudioLanguage_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x11, 0x00 };
		const UL SecondaryAudioLanguage_UL(SecondaryAudioLanguage_UL_Data);

		const UInt8 SeriesTitle_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x07, 0x01, 0x0b, 0x01, 0x01, 0x01 };
		const UL SeriesTitle_UL(SeriesTitle_UL_Data);

		const UInt8 ShimName_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x01, 0x07, 0x01, 0x0b, 0x01, 0x01, 0x04 };
		const UL ShimName_UL(ShimName_UL_Data);

		const UInt8 SignLanguage_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x1f, 0x00 };
		const UL SignLanguage_UL(SignLanguage_UL_Data);

		const UInt8 SigningPresent_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x1e, 0x00 };
		const UL SigningPresent_UL(SigningPresent_UL_Data);

		const UInt8 Synopsis_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x02, 0x00 };
		const UL Synopsis_UL(Synopsis_UL_Data);

		const UInt8 TertiaryAudioLanguage_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x12, 0x00 };
		const UL TertiaryAudioLanguage_UL(TertiaryAudioLanguage_UL_Data);

		const UInt8 TextlessElementsExist_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x21, 0x00 };
		const UL TextlessElementsExist_UL(TextlessElementsExist_UL_Data);

		const UInt8 ThreeD_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x0a, 0x00 };
		const UL ThreeD_UL(ThreeD_UL_Data);

		const UInt8 ThreeDType_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x0b, 0x00 };
		const UL ThreeDType_UL(ThreeDType_UL_Data);

		const UInt8 TotalNumberOfParts_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x17, 0x00 };
		const UL TotalNumberOfParts_UL(TotalNumberOfParts_UL_Data);

		const UInt8 TotalProgrammeDuration_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x18, 0x00 };
		const UL TotalProgrammeDuration_UL(TotalProgrammeDuration_UL_Data);

		const UInt8 UKDPPFramework_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x02, 0x53, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00 };
		const UL UKDPPFramework_UL(UKDPPFramework_UL_Data);

		const UInt8 UKDPPSchemeLabel_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x04, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00 };
		const UL UKDPPSchemeLabel_UL(UKDPPSchemeLabel_UL_Data);

		const UInt8 VideoComments_UL_Data[16] = { 0x06, 0x0e, 0x2b, 0x34, 0x01, 0x01, 0x01, 0x01, 0x0d, 0x0c, 0x01, 0x01, 0x01, 0x01, 0x10, 0x00 };
		const UL VideoComments_UL(VideoComments_UL_Data);

	} // namespace AS_11
