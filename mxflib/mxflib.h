/*! \file	mxflib.h
 *	\brief	The main MXFLib header file
 *
 *	\version $Id$
 *
 */
/*
 *	Copyright (c) 2003, Matt Beard
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
 */

#ifndef MXFLIB__MXFLIB_H
#define MXFLIB__MXFLIB_H

//! Namespace for all MXFLib items
namespace mxflib {}


//! Include system specifics first to allow any required compiler set-up
#include <mxflib/system.h>


// Required std::headers

#include <list>
#include <map>



#include <mxflib/debug.h>

#include <mxflib/smartptr.h>

#include <mxflib/endian.h>

#include <mxflib/types.h>

#include <mxflib/datachunk.h>

#include <mxflib/forward.h>




#include <mxflib/helper.h>


#include <mxflib/mdtraits.h>
#include <mxflib/mdtype.h>
#include <mxflib/deftypes.h>

#include <mxflib/mdobject.h>

#include <mxflib/metadata.h>

#include <mxflib/rip.h>

#include <mxflib/mxffile.h>

#include <mxflib/index.h>

#include <mxflib/essence.h>

#include <mxflib/klvobject.h>

#include <mxflib/crypto.h>

#include <mxflib/esp_mpeg2ves.h>
#include <mxflib/esp_wavepcm.h>
#include <mxflib/esp_dvdif.h>

#include <mxflib/waveheader.h>

#endif // MXFLIB__MXFLIB_H

