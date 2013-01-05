/*! \file	mxflib_assert.h.h
 *	\brief	Wrapper around ASSERT to allow extra debug
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

#ifndef __MXFLIB_ASSERT_H__
#define  __MXFLIB_ASSERT_H__

#define mxflib_assert(exp) ( ASSERT(exp) )
#define mxflib_assert_fatal(exp, msg) (void)( (exp) || (ASSERT(exp), error("%s", msg), exit(-1)) )

#endif  // __MXFLIB_ASSERT_H__

