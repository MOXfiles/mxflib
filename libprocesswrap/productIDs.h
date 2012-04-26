/*! \file	productIDs.h
 *	\brief	product identification header
 */
/*
 *	Copyright (c) 2005, Metaglue Corporation
 *
 *	This software is provided 'as-is', without any express or implied warranty.
 *	In no event will the authors be held liable for any damages arising from
 *	the use of this software.
 *
 *	Permission is granted to anyone to use this software for any purpose,
 *	including commercial applications, and to alter it and redistribute it
 *	freely, subject to the following restrictions:
 *
 *	  1. The origin of this software must not be misrepresented; you must
 *	     not claim that you wrote the original software. If you use this
 *	     software in a product, an acknowledgment in the product
 *	     documentation would be appreciated but is not required.
 *	
 *	  2. Altered source versions must be plainly marked as such, and must
 *	     not be misrepresented as being the original software.
 *	
 *	  3. This notice may not be removed or altered from any source
 *	     distribution.
 *
 */

// Product UUID and version text for this release
// {B658A3F9-280D-463c-9C12-FD6DAA6E1213}
UInt8 Product_UUID_Data[16] = { 0xb6, 0x58, 0xa3, 0xf9, 0x28, 0x0d, 0x46, 0x3c, 0x9c, 0x12, 0xfd, 0x6d, 0xaa, 0x6e, 0x12, 0x13 };

mxflib::UUID Product_UUID(Product_UUID_Data);

std::string CompanyName = "freeMXF.org";

std::string ProductName = "MXFWrap File Wrapper";


#ifndef PRODUCT_VERSION_MAJOR
#define PRODUCT_VERSION_MAJOR "1"
#define PRODUCT_VERSION_MINOR "0"
#define PRODUCT_VERSION_TWEAK "2"
#define PRODUCT_VERSION_BUILD "0"
#define PRODUCT_VERSION_REL 4
#endif

// Convert the above into suitable form for setting the Ident
// Convention for version string is
//
//		<major>.<minor>.<tertiary>.<patch> <kind>
//
// where <kind> is
//
//		1	Release
//		2	Development
//		3	Patched
//		4	Beta
//		5	Private

std::string ProductVersionString  = std::string("V"   PRODUCT_VERSION_MAJOR "." PRODUCT_VERSION_MINOR "."
			                                     PRODUCT_VERSION_TWEAK "." PRODUCT_VERSION_BUILD )
			                 + std::string(      MXFLIB_VERSION_RELTEXT(PRODUCT_VERSION_REL) );

std::string ProductProductVersion = std::string( "\"" PRODUCT_VERSION_MAJOR "\",\"" PRODUCT_VERSION_MINOR "\",\"" 
			                                     PRODUCT_VERSION_TWEAK "\",\"" PRODUCT_VERSION_BUILD "\",\"")
							 + std::string(      MXFLIB_VERSION_RELNUMBER(PRODUCT_VERSION_REL)) + std::string("\"");
