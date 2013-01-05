/*! \file	productIDs.h
 *	\brief	product identification header
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

// Product UUID and name
// {A924C5D4-BD7B-486d-AECD-DEB7A36E493E}
UInt8 Product_UUID_Data[16] = { 0xa9, 0x24, 0xc5, 0xd4, 0xbd, 0x7b, 0x48, 0x6d, 0xae, 0xcd, 0xde, 0xb7, 0xa3, 0x6e, 0x49, 0x3e };
mxflib::UUID Product_UUID(Product_UUID_Data);

std::string CompanyName = "freeMXF.org";

std::string ProductName = "MXFSplit File Splitter";

// version text for this release

#ifndef PRODUCT_VERSION_MAJOR
#define PRODUCT_VERSION_MAJOR "3"
#define PRODUCT_VERSION_MINOR "0"
#define PRODUCT_VERSION_TWEAK "0"
#define PRODUCT_VERSION_BUILD "0"
#endif

#ifndef PRODUCT_VERSION_REL
#ifdef NDEBUG
#define PRODUCT_VERSION_REL 4
#else // NDEBUG
#define PRODUCT_VERSION_REL 5
#endif // NDEBUG
#endif // PRODUCT_VERSION_REL

// Convert the above into suitable form for setting the Ident
// Convention for version string is
//
//		<major>.<minor>.<tertiary>.<patch>(<kind>)
//
// where <kind> is
//
//		1 = Release = automated Release build on dedicated machine
//		2 = Development = automated or manual Debug build on dedicated machine
//		3 = Patch = manual repeat Release build on dedicated machine
//		4 = Beta = Release build on individual machine
//		5 = Private = Debug build on individual machine

std::string ProductVersionString  = std::string("V"   PRODUCT_VERSION_MAJOR "." PRODUCT_VERSION_MINOR "."
			                                     PRODUCT_VERSION_TWEAK "." PRODUCT_VERSION_BUILD )
			                 + std::string(      MXFLIB_VERSION_RELTEXT(PRODUCT_VERSION_REL) );

std::string ProductProductVersion = std::string( "\"" PRODUCT_VERSION_MAJOR "\",\"" PRODUCT_VERSION_MINOR "\",\"" 
			                                     PRODUCT_VERSION_TWEAK "\",\"" PRODUCT_VERSION_BUILD "\",\"")
							 + std::string(      MXFLIB_VERSION_RELNUMBER(PRODUCT_VERSION_REL)) + std::string("\"");
