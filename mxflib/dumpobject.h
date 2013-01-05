/*! \file	dumpobject.h
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
#ifndef _dumpobject_h_
#define _dumpobject_h_

#include "mxflib/mxflib.h"
using namespace mxflib;

#include <stdio.h>
#include <iostream>
using namespace std;

namespace mxflib {

	//! types of XML output format
	enum XML_Type_t {
		XML_Type_None,
		XML_Type_Default,
		XML_Type_AAFx,
		XML_Type_AAF_XML,	// pre-std RegXML per AAFSDK 1.1.4
		XML_Type_MXFixer,
		XML_Type_RegXML,	// SMPTE 2001 once it is approved
		XML_Type_SMPTE_434
	};

	//! XML output options struct
	typedef struct _XML_options_t {

		XML_Type_t XMLType;

	} XML_options_t;

	//! Dump an MDObject
	void DumpObject( MDObjectPtr Object, std::string PrefixString, bool bare = false );

} // namespace mxflib

#endif // _dumpobject_h_
