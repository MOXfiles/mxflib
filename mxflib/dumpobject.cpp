/*! \file	dumpobject.cpp
 *	\brief	Dump an MDObject
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
#include "dumpobject.h"

#include "utility/utility.h"
using namespace utility;

#include "mxflib/mxflib.h"
using namespace mxflib;

#include <stdio.h>
#include <iostream>
using namespace std;

namespace mxflib {

// TODO convert these into arguments
// from mxfx
static XML_Type_t XMLType=XML_Type_MXFixer; // -xg

static bool CollapseSingleSequence = false;	// -xc Collapse Sequence of single segment to a Segment
static bool SkipStrongSingleton = false;	// -xs Skip showing the class of a singleton strong ref as a separate element as in RegXML

// from mxfdump
//! Flag for dumping object locations as well as object data
static bool DumpLocation = false;

//! Flag for following global references
static bool FollowGlobals = true;

//! Flag for noting if IsModified()
static bool DumpModified = false;

//! Flag for alpha sorted dump
static bool SortedDump = false;

#ifdef OPTION3ENABLED
//! Flag for diplaying baseline UL of sets using the ObjectClass extension mechanism
static bool ShowBaseline = false;
#endif // OPTION3ENABLED

// maximum value size to dump
// above this, dump will just state size
#define MAX_DUMPSIZE 128

// merged from XMLMetaParser, mxfsplit, mxfdump

//! Dump an MDObject and any physical or logical children
void DumpObject( MDObjectPtr Object, std::string Prefix, bool bare /* = false */ )
{
	if(DumpLocation) printf("%s0x%s:\n", Prefix.c_str(), Int64toHexString(Object->GetLocation(),8).c_str());

	if(DumpModified && Object->IsModified()) printf("%s%s is *MODIFIED*\n", Prefix.c_str(), Object->FullName().c_str() );

#ifdef OPTION3ENABLED
	if(ShowBaseline)
	{
		if(!Object->IsBaseline())
		{
			if(Object->GetBaselineUL())
			{
				MDOTypePtr BaselineClass = MDOType::Find(Object->GetBaselineUL());
				if(BaselineClass)
				{
					printf("%sBaseline: %s\n", Prefix.c_str(), BaselineClass->Name().c_str());
				}
				else
				{
					printf("%sNote: Current dictionary does not contain a set with the baseline UL used to wrap this non-baseline class\n", Prefix.c_str());
					printf("%sBaseline: %s\n", Prefix.c_str(), Object->GetBaselineUL()->GetString().c_str());
				}
				Prefix += "  ";
			}
			else
			{
				printf("%sNote: Current dictionary flags this class as non-baseline, but it is not wrapped in a baseline class\n", Prefix.c_str());
			}
		}
		else
		{
			if(Object->GetBaselineUL())
			{
				printf("%sNote: Current dictionary flags this class as baseline, but it is wrapped as a non-baseline class\n", Prefix.c_str());

				MDOTypePtr BaselineClass = MDOType::Find(Object->GetBaselineUL());
				if(BaselineClass)
				{
					printf("%sBaseline: %s\n", Prefix.c_str(), BaselineClass->Name().c_str());
				}
				else
				{
					printf("%sNote: Current dictionary does not contain a set with the baseline UL used to wrap this non-baseline class\n", Prefix.c_str());
					printf("%sBaseline: %s\n", Prefix.c_str(), Object->GetBaselineUL()->GetString().c_str());
				}
				Prefix += "  ";
			}
		}
	}
#endif // OPTION3ENABLED

	if(Object->GetLink())
	{
		if(Object->GetRefType() == ClassRefStrong)
		{
			if(Object->EffectiveRefNested())
			{
				printf("%s%s -> Nested Item\n", Prefix.c_str(), Object->Name().c_str(), Object->GetLink()->Name().c_str());

				DumpObject(Object->GetLink(), Prefix + "  ");
			}
			else
			{
				printf("%s%s = %s\n", Prefix.c_str(), Object->Name().c_str(), Object->GetString().c_str());
	
				if(DumpLocation) printf("%s0x%s:\n", Prefix.c_str(), Int64toHexString(Object->GetLocation(),8).c_str());
				printf("%s%s -> Strong Reference to %s\n", Prefix.c_str(), Object->Name().c_str(), Object->GetLink()->Name().c_str());
	
				DumpObject(Object->GetLink(), Prefix + "  ");
			}
		}
		else if(Object->GetRefType() == ClassRefGlobal)
		{
			if(FollowGlobals)
			{
				printf("%s%s = %s\n", Prefix.c_str(), Object->Name().c_str(), Object->GetString().c_str());

				if(DumpLocation) printf("0x%s : ", Int64toHexString(Object->GetLocation(),8).c_str());
				printf("%s%s -> Global Reference to %s\n", Prefix.c_str(), Object->Name().c_str(), Object->GetLink()->Name().c_str());

				DumpObject(Object->GetLink(), Prefix + "  ");
			}
			else
			{
				printf("%s%s -> Global Reference to %s, %s\n", Prefix.c_str(), Object->Name().c_str(), Object->GetLink()->Name().c_str(), Object->GetString().c_str());
			}
		}
		else if(Object->GetRefType() == ClassRefMeta)
		{
			std::string TargetName = Object->GetLink()->GetString(MetaDefinitionName_UL, Object->GetLink()->Name());
			printf("%s%s -> MetaDictionary Reference to %s %s\n", Prefix.c_str(), Object->Name().c_str(), TargetName.c_str(), Object->GetString().c_str());
		}
		else if(Object->GetRefType() == ClassRefDict)
		{
			std::string TargetName = Object->GetLink()->GetString(DefinitionObjectName_UL, Object->GetLink()->Name());
			printf("%s%s -> Dictionary Reference to %s %s\n", Prefix.c_str(), Object->Name().c_str(), TargetName.c_str(), Object->GetString().c_str());
		}
		else
		{
			printf("%s%s -> Weak Reference to %s %s\n", Prefix.c_str(), Object->Name().c_str(), Object->GetLink()->Name().c_str(), Object->GetString().c_str());
		}
	}
	else
	{
		if(Object->IsDValue())
		{
			printf("%s%s = <Unknown>\n", Prefix.c_str(), Object->Name().c_str());
		}
		else
		{
			// Check first for values that are not reference batches
			if(Object->IsAValue())
			{
				if( Object->Value->GetData().Size > MAX_DUMPSIZE )
				{
					if(sizeof(size_t) == 4)
                        printf("%s%s = RAW[0x%08x]", Prefix.c_str(), Object->Name().c_str(), (int)Object->Value->GetData().Size );
					else
						printf("%s%s = RAW[0x%s]", Prefix.c_str(), Object->Name().c_str(), Int64toHexString(Object->Value->GetData().Size, 8).c_str() );

					const unsigned char* p = Object->Value->GetData().Data;
					int i; for(i=0;i<3;i++)
					{
						printf("\n%s%*c      ", Prefix.c_str(), (int)strlen(Object->Name().c_str()), ' ');
						int j; for(j=0;j<4;j++)
						{
							int k; for(k=0;k<4;k++) printf("%02x", *p++);
							printf(" ");
						}
						if(i==2) printf( "...\n" );
					}
				}
				else
				{
					if( Object->Name().find("Unknown") == std::string::npos )
						printf("%s%s = %s\n", Prefix.c_str(), Object->Name().c_str(), Object->GetString().c_str());
					else
						printf("%s%s\n", Prefix.c_str(), Object->Name().c_str());

					if( Object->GetRefType() == ClassRefMeta )
						printf("%s%s is an unsatisfied MetaRef\n", Prefix.c_str(), Object->Name().c_str());
					else if( Object->GetRefType() == ClassRefDict )
						printf("%s%s is an unsatisfied DictRef\n", Prefix.c_str(), Object->Name().c_str());
				}
			}
			else
			{
				printf("%s%s\n", Prefix.c_str(), Object->Name().c_str());

				MDObjectULList::iterator it = Object->begin();

				if(!SortedDump)
				{
					/* Dump Objects in the order stored */
					while(it != Object->end())
					{
						DumpObject((*it).second, Prefix + "  ");
						it++;
					}
				}
				else
				{
					/* Dump Objects in alphabetical order - to allow easier file comparisons */
					std::multimap<std::string, MDObjectPtr> ChildMap;
					std::multimap<std::string, MDObjectPtr>::iterator CM_Iter;

					while(it != Object->end())
					{
						ChildMap.insert(std::multimap<std::string, MDObjectPtr>::value_type((*it).second->Name(), (*it).second));
						it++;
					}

					CM_Iter = ChildMap.begin();
					while(CM_Iter != ChildMap.end())
					{
						DumpObject((*CM_Iter).second, Prefix + "  ");
						CM_Iter++;
					}
				}
			}
		}
	}

	return;
}

} // namespace mxflib
