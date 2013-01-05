/*! \file	parserxi.cpp
 *	\brief	RXI format dictionary parser example
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

/* NOTE: This example requires the RXI parser code from MXFLib */
#include "mxflib/mxflib.h"
using namespace mxflib;

//! Print debug dump of classes and types
void OutputDebug(RXIDataPtr &Data);

namespace
{
	//! Set true for verbose mode
	bool DebugMode = false;
}

//! Example of parsing an RXI file
int main(int argc, char *argv[]) 
{
	if(argc < 2)
	{
		fprintf(stderr, "Example RXI file parser\n\n");
		
		fprintf(stderr, "Usage:\n");
		fprintf(stderr, "    %s [-v] <filename> [<application>]\n\n", argv[0]);
		
		fprintf(stderr, "Where -v is verbose mode and <application> format is:\n");
		fprintf(stderr, "    Filename      = Most recent version of application called \"Application\"\n");
		fprintf(stderr, "    Filename[ver] = Specified version of application called \"Application\"\n");

		fprintf(stderr, "Note: Appending a ~ after the application will enable alias substitution\n\n");
		
		fprintf(stderr, "Example:\n");
		fprintf(stderr, "    %s reg.rxi AAF[1.2]~\n\n", argv[0]);

		fprintf(stderr, "    This will parse \"reg.rxi\" and display all items that are valid for\n");
		fprintf(stderr, "    application version 1.2 of AAF. Any entries that have an alias name\n");
		fprintf(stderr, "    specified for this version of AAF will be renamed accordingly\n");

		return 1;
	}

	std::string Filename;
	std::string Application;
	if((argv[1][0] == '-') && (tolower(argv[1][1]) == 'v'))
	{
		DebugMode = true;
		Filename = (argc > 2) ? argv[2] : "No Filename";
		Application = (argc > 3) ? argv[3] : "";
	}
	else
	{
		Filename = argv[1];
		Application = (argc > 2) ? argv[2] : "";
	}

	RXIDataPtr Data = ParseRXIFile(Filename, Application);

	if(!Data)
	{
		error("Unable to parse RXI file \"%s\"\n", Filename.c_str());
		return 2;
	}
	else if(Data->LegacyFormat)
	{
		error("\"%s\" is not an RXI file\n", Filename.c_str());
		return 3;
	}

	OutputDebug(Data);

	return 0;
}



//! Debug function to dump a list of classes
void DumpClasses(ClassRecordList Classes, std::string Prefix = "")
{
	ClassRecordList::iterator it = Classes.begin();
	while(it != Classes.end())
	{
		printf("%s%s%s ", Prefix.c_str(), ((*it)->SymSpace ? ((*it)->SymSpace->Name() + "::").c_str() : ""), (*it)->Name.c_str());

		switch((*it)->Class)
		{
			default: printf("[Unknown %d]", (int)(*it)->Class); break;
			case ClassRename: printf("[Rename of %s] ", (*it)->Base.c_str());
			case ClassPack:
			case ClassSet: 
				if((*it)->MaxSize == DICT_LEN_NONE)
				{
					printf(((*it)->MaxSize == 0) ? "[DefinedLengthPack" : "[VariableLengthPack:"); break;
				}
				else
				{
					printf("[Set %s:",
					       (*it)->MinSize == DICT_KEY_1_BYTE ? "1" :
						   (*it)->MinSize == DICT_KEY_2_BYTE ? "2" :
						   (*it)->MinSize == DICT_KEY_4_BYTE ? "4" :
						   (*it)->MinSize == DICT_KEY_AUTO ? "Auto(16)" :
						   (*it)->MinSize == DICT_KEY_GLOBAL ? "Global" : "Unknown"
						  );
				}
				if((*it)->MaxSize != DICT_LEN_NONE)
				{
					printf("%s]",
						   (*it)->MaxSize == DICT_LEN_1_BYTE ? "1" : 
						   (*it)->MaxSize == DICT_LEN_2_BYTE ? "2" : 
						   (*it)->MaxSize == DICT_LEN_4_BYTE ? "4" : 
						   (*it)->MaxSize == DICT_LEN_BER ? "BER" : "Unknown"
				  		  );
				}
				else printf("]");

				break;
			case ClassVector: printf("[Vector of %s]", (*it)->Base.c_str()); break;
			case ClassArray: printf("[Array of %s]", (*it)->Base.c_str()); break;
			case ClassItem: printf("[Item of type %s]", (*it)->Base.c_str()); break;
		}
		if(((*it)->Class == ClassPack) || ((*it)->Class == ClassSet))
		{
			if(!(*it)->Base.empty()) printf(" Based on group %s,", (*it)->Base.c_str());
		}
		printf(" UL=%s, ", (*it)->UL->GetString().c_str());
		printf("\n");

		if(!(*it)->Children.empty()) DumpClasses((*it)->Children, Prefix + "  ");

		it++;
	}
}


//! Debug function to dump a list of types
void DumpTypes(TypeRecordList Types, std::string Prefix = "")
{
	TypeRecordList::iterator it = Types.begin();
	while(it != Types.end())
	{
		printf("%s%s ", Prefix.c_str(), (*it)->Type.c_str());
		switch((*it)->Class)
		{
			default: printf("[Unknown %d]", (int)(*it)->Class); break;
			case TypeBasic: printf("[Basic(%d-byte Integer)]", (*it)->Size); break;
			case TypeInterpretation: printf("[Rename of %s]", (*it)->Base.c_str()); break;
			case TypeMultiple: printf("[%s of %s]", (*it)->ArrayClass == ARRAYEXPLICIT ? "Set" : "Array", (*it)->Base.c_str()); break;
			case TypeCompound: printf("[Record]"); break;
			case TypeEnum: printf("[Enumeration based on %s]", (*it)->Base.c_str()); break;
			case TypeSub: printf("[Sub-Item of type %s]", (*it)->Base.c_str()); break;
		}
		if((*it)->UL) printf(" UL=%s", (*it)->UL->GetString().c_str());
		printf("\n");

		if(!(*it)->Children.empty()) DumpTypes((*it)->Children, Prefix + "  ");

		it++;
	}
}


//! Debug function to dump a list of labels
void DumpLabels(TypeRecordList Labels, std::string Prefix = "")
{
	TypeRecordList::iterator it = Labels.begin();
	while(it != Labels.end())
	{
		printf("%s%s UL=%s\n", Prefix.c_str(), (*it)->Type.c_str(), (*it)->UL->GetString().c_str());
		it++;
	}
}


//! Print debug dump of classes and types
void OutputDebug(RXIDataPtr &Data)
{
	printf("\nTypes:");
	printf("\n======\n");
	DumpTypes(Data->TypesList);

	printf("\nGroups:");
	printf("\n=======\n");
	DumpClasses(Data->GroupList);

	printf("\nElements not in Groups:");
	printf("\n=======================\n");
	DumpClasses(Data->ElementList);

	printf("\nLabels:");
	printf("\n=======\n");
	DumpLabels(Data->LabelsList);
}



// Debug and error messages
#include <stdarg.h>

#ifdef MXFLIB_DEBUG
//! Display a general debug message
void mxflib::debug(const char *Fmt, ...)
{
	if(!DebugMode) return;

	va_list args;

	va_start(args, Fmt);
	vprintf(Fmt, args);
	va_end(args);
}
#endif // MXFLIB_DEBUG

//! Display a warning message
void mxflib::warning(const char *Fmt, ...)
{
	va_list args;

	va_start(args, Fmt);
	printf("Warning: ");
	vprintf(Fmt, args);
	va_end(args);
}

//! Display an error message
void mxflib::error(const char *Fmt, ...)
{
	va_list args;

	va_start(args, Fmt);
	printf("ERROR: ");
	vprintf(Fmt, args);
	va_end(args);
}
