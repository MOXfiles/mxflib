/*! \file	IPartial.h
 *	\brief	unwrap library for MXFLib
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
#ifndef _IPartial_H_
#define _IPartial_H_

#include "mxflib/mxflib.h"
using namespace mxflib;

namespace mxflib
{
	//! Interface to add to extensions of EssenceSink that may be a PartialSink and so may end early
	class IPartial
	{
	public:
		//! Has this partial clip has ended
		virtual bool EndOfPartial(void) = 0;
	};

	//! Class to allow partial extraction of essence - passes on a given range of edit units to another sink
	class PartialSink : public EssenceSink, public IPartial
	{
	private:
		PartialSink();									//!< Prevent default construction

	protected:
		EssenceSinkPtr Sink;							//!< The EssenceSink to receive the range
		Position Start;									//!< Position of the first edit unit to extract
		Length Count;									//!< Number of edit units to extract
		Position Pos;									//!< Current position in edit units

	public:
		//! Initialize this sink
		PartialSink(EssenceSinkPtr TargetSink, Position StartPos, Length RangeLength) : Sink(TargetSink), Start(StartPos), Count(RangeLength), Pos(0)
		{
		}

		//! Receive the next "installment" of essence data
		/*! This will receive a buffer containing the next bytes of essence data
		 *  \param Buffer The data buffer
		 *  \param BufferSize The number of bytes in the data buffer
		 *  \param EndOfItem This buffer is the last in this wrapping item
		 *  \return True if all is OK, else false
		 *  \note The first call may well fail if the sink has not been fully configured.
		 *	\note If false is returned the caller should make no more calls to this function, but the function should be implemented such that it is safe to do so
		 */
		virtual bool PutEssenceData(UInt8 const *Buffer, size_t BufferSize, bool EndOfItem = true)
		{
			bool Ret = true;

			if((Pos >= Start) && (Pos < (Start + Count)))
			{
				Ret = Sink->PutEssenceData(Buffer, BufferSize, EndOfItem);
			}

			if(EndOfItem) Pos++;

			return Ret;
		}

		//! Called once all data exhausted
		/*! \return true if all is OK, else false
		 *  \note This function must also be called from the derived class' destructor in case it is never explicitly called
		 */
		virtual bool EndOfData(void) { return Sink->EndOfData(); }


		/* IPartial */

		//! Has this partial clip has ended
		bool EndOfPartial(void)
		{
			// DRAGONS: We actually wait until 1 edit unit after the end of the clip so that any other streams will have extracted their last data
			if(Pos > (Start + Count)) return true;
			return false;
		}
	};
}
#endif // _IPartial_H_
