/*! \file	timecode.cpp
 *	\brief	Timecode handling class for MXF 
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


#include "timecode.h"

#ifndef EXCLUDE_MXFLIB_H
using namespace mxflib;
#endif

	// construct from string
	// note that defaults to 1000 fps - intentionally
	// MUST call SetFPS() before persisting
	Timecode_t::Timecode_t( const char* arg, unsigned fps /* = defaultfps */, bool drop /*= false*/ ) : _start(0),_valid(false),_fps(fps),_drop(drop),_text(NULL)
	{
		if((!arg) || !strlen(arg)) return;

		// Set the value, and infer the FPS and drop if we have not been given values
		Set(arg, (fps == defaultfps));
	}

	// construct from  unsigned, fps and drop flag
	// note that defaults to 1000 fps - MUST call SetFPS() before persisting
	Timecode_t::Timecode_t( MXFLIB_TYPE(Position) arg /* = 0 */, const unsigned fps /* = defaultfps */, const bool drop /* = false */ )
		: _start(arg),_valid(false),_fps(fps),_drop(drop),_text(NULL)
	{
		/*	bad idea here - compensation has already been done

			if( _drop && _fps!=defaultfps )
			{
				MXFLIB_TYPE(Position) mins = _start / (60*_fps);
				MXFLIB_TYPE(Position) tenmins = mins/10;
				_start -= 2*(mins-tenmins);
			}
		*/

		if( _fps!=defaultfps ) _valid = true;
	}

	// construct from  Hours, Minutes, Seconds, Frames, fps and drop flag  (and default)
	// note that defaults to 1000 fps - MUST call SetFPS() before persisting
	Timecode_t::Timecode_t( const unsigned h, const unsigned m, const unsigned s, const unsigned f, const unsigned fps /*= defaultfps */, const bool drop /*= false */ )
		: _start(0),_valid(false),_fps(fps),_drop(drop),_text(NULL)
	{
		Set( h,m,s,f );
		if( _fps!=defaultfps ) _valid = true;
	}

	//! Set the value from a string, optionally inferring the FPS and drop (return true if OK)
	bool Timecode_t::Set( const char *arg, bool InferFPS /*=false*/)
	{
		if((!arg) || !strlen(arg))
		{
			_start = 0;
			_valid = false;
			return _valid;
		}

		// arg is of the form hhpmmpssqff
		// hh mm ss ff are decimal digits
		// q is ? + .,:; -~ /\=#
		// if q is , or ; or ~ set _drop

		// NOTE that if fps is specified as an arg, it overrides the punctuator

		// if q is + set _fps(24)
		// if q is -~ set _fps(60)
		// if q is .,:; set _fps(30)
		// if q is / set _fps(25)
		// if q is = set _fps(50)
		// if q is # set _fps(100)

		// if q is doubled, means true integer frame rate, not *1000/1001
		// today, there is actually no difference
		// also, today field 2 makes no difference

		//	?	unknown

		//	+	23.976 (actually 24 * 1000/1001)

		//	.	29.97 n field 1 (actually 30 * 1000/1001)
		//	:	29.97 n field 2
		//	,	29.97 d field 1
		//	;	29.97 d field 2
		//	-	59.94 n (actually 60 * 1000/1001)
		//	~	59.94 d

		//	/	25 field 1
		//	\	25 field 2
		//	=	50			(= think 2 x 25)
		//	#	100			(# think 4 x 25)

		//	++	24

		//	..	30 n field 1 (actually 30)
		//	::	30 n field 2
		//	,,	30 d field 1
		//	;;	30 d field 2
		//	--	60 n (actually 60)
		//	~~	60 d

		// in future, we will also need methodology to override the punctuator
		// by using a suffix to the string, something like "@60p" "*60p" "%60p"

		/* 
			NOTE this is not the same approach as in Brooks Harris "TCF" which is much more extensive
			and uses two separators and allows for a bundle of other features 
			that said, many (but not all) of the q puncutators are compatible
		*/

		unsigned mult = 1;
		unsigned seps = 0;
		const char* pc = arg + strlen(arg);
		
		unsigned int InferredFPS = defaultfps;
		bool InferredDrop = false;

		_start = 0;
		while( pc!=arg )
		{
			char c = *--pc;

			if( c >='0' && c <='9' )
			{
				_start += mult*(c-'0');
				mult*=10;
			}
			else
			{
				if( (++seps) == 1 )
				{
					switch(c)
					{
					case '+':
						InferredFPS = 24;
						InferredDrop = false;
						break;
					case ';':
						// TODO set field 2 and fall through
					case ',':
						InferredFPS = 30;
						InferredDrop = true;
						break;
					case ':':
						// TODO set field 2 and fall through
					case '.':
						InferredFPS = 30;
						InferredDrop = false;
						break;
					case '~':
						InferredFPS = 60;
						InferredDrop = true;
						break;
					case '-':
						InferredFPS = 60;
						InferredDrop = false;
						break;
					case '\\':
						// TODO set field 2 and fall through
					case '/':
						InferredFPS = 25;
						InferredDrop = false;
						break;
					case '=':
						InferredFPS = 50;
						InferredDrop = false;
						break;
					case '#':
						InferredFPS = 100;
						InferredDrop = false;
						break;
					default:
						break;
					}
					
					// Set the FPS and drop from the format - if requested
					if(InferFPS)
					{
						_fps = InferredFPS;
						_drop = InferredDrop;
					}

					mult=_fps;

					// intercept double punctuator
					if( pc!=arg && *(pc-1)==c )
					{
						pc--;
						// TODO clear future *1000/1001 flag
					}
				}
				else if( seps==2 )
				{
					mult=60*_fps; // 60 secs per min
				}
				else if( seps==3 )
				{
					mult=60*60*_fps; // 60 mins per hour
				}
				else if( seps==4 )
				{
					mult=24*60*60*_fps; // 24 hours per day
				}
			}
		}
		if( _drop )
		{
			MXFLIB_TYPE(Position) mins = _start / (60*_fps);
			MXFLIB_TYPE(Position) tenmins = mins/10;
			_start -= 2*(mins-tenmins);
		}
		_valid = true;

		return _valid;
	}


	//! Set from Hours, Minutes, Seconds, Frames
	void Timecode_t::Set( const unsigned Hours, const unsigned Minutes, const unsigned Seconds, const unsigned Frames )
	{
		_start = Frames + (_fps * Seconds) + (60 * _fps * Minutes) + (60 * 60 * _fps * Hours);
		if( _drop && _fps!=defaultfps )
		{
			MXFLIB_TYPE(Position) mins = _start / (60*_fps);
			MXFLIB_TYPE(Position) tenmins = mins/10;
			_start -= 2*(mins-tenmins);
		}
	}


	// Set the frames per second indicator and update the framecount accordingly
	void Timecode_t::SetFPS( unsigned fps, bool drop )
	{
		// do NOT alter _valid
		if( _fps!=defaultfps )
		{
			// convert from whatever fps to defaultfps
			MXFLIB_TYPE(Position) frames = _start % _fps;
			MXFLIB_TYPE(Position) seconds = _start / _fps;
			_fps = defaultfps;
			_start = seconds*_fps + frames;
		}
		// convert from defaultfps to desired fps
		MXFLIB_TYPE(Position) frames = _start % _fps;
		MXFLIB_TYPE(Position) seconds = _start / _fps;
		_fps = fps;
		_start = seconds*_fps + frames;

		_drop=drop;
		if( _drop )
		{
			MXFLIB_TYPE(Position) mins = _start / (60*_fps);
			MXFLIB_TYPE(Position) tenmins = mins/10;
			_start -= 2*(mins-tenmins);
		}
	}

	// Set to a frame count
	// does NOT affect _drop
	void Timecode_t::Set( MXFLIB_TYPE(Position) start, unsigned fps /* = defaultfps */ )
	{
		// do not unwittingly clobber _fps
		if( fps!=defaultfps )
		{
			if( _fps!=defaultfps ) _fps=fps;
		}

		_start=start; 
	}

	// Intermediate function that calculates H:M:S:F
	// Returns an unsigned int with HMSF as unsigned ints in each byte

	// DRAGONS this only gives the right answer up to 255 hours (10.5 days)
	//		   beyond that will always say 255
	unsigned int Timecode_t::toHMSF()
	{
		unsigned int result=0;
		unsigned int  fps=_fps;
		if( fps==0 ) fps++;

		MXFLIB_TYPE(Position) f = _start;

		unsigned int hours, minutes, seconds, frames;

		// drop frame readjustment
		// see "FRAMEtoDFtoFRAME.xls" for verification
		if( _drop && _fps==30 )
		{
			MXFLIB_TYPE(Position) tm   = f / 17982;
			MXFLIB_TYPE(Position) f_tm = f % 17982;
			MXFLIB_TYPE(Position) m    = f_tm / 1798;
			MXFLIB_TYPE(Position) f_m  = f_tm % 1798;

			MXFLIB_TYPE(Position) f_ma = f_m;
			if( f_m<2 && m>0 ) f_ma += 1798;

			MXFLIB_TYPE(Position) ma   = tm*10 + m;
			if( f_m<2 && m>0 ) ma -= 1;
			
			hours = static_cast<unsigned int>((tm / 6));
			minutes =static_cast<unsigned int>(ma % 60);
			seconds = static_cast<unsigned int>(f_ma / fps);
			frames = static_cast<unsigned int>(f_ma % fps);
		}
		else if( _drop && _fps==60 )
		{
			// TODO is DF counting actually used for 60p ? if so, is this correct ?
			MXFLIB_TYPE(Position) tm   = f / (17982*2);
			MXFLIB_TYPE(Position) f_tm = f % (17982*2);
			MXFLIB_TYPE(Position) m    = f_tm / (1798*2);
			MXFLIB_TYPE(Position) f_m  = f_tm % (1798*2);

			MXFLIB_TYPE(Position) f_ma = f_m;
			if( f_m<2 && m>0 ) f_ma += (1798*2);

			MXFLIB_TYPE(Position) ma   = tm*10 + m;
			if( f_m<2 && m>0 ) ma -= 1;
			
			hours = static_cast<unsigned int>((tm / 6));
			minutes = static_cast<unsigned int>(ma % 60);
			seconds = static_cast<unsigned int>(f_ma / fps);
			frames = static_cast<unsigned int>(f_ma % fps);
		}
		else
		{
			frames = static_cast<unsigned int>(f % fps);
			MXFLIB_TYPE(Position) s = static_cast<unsigned int>(f / fps);

			MXFLIB_TYPE(Position) m = static_cast<unsigned int>(s / 60);
			seconds = static_cast<unsigned int>(s % 60);

			hours = static_cast<unsigned int>((m / 60));
			minutes = static_cast<unsigned int>(m % 60);
		}

		result  = ( (hours>255)?(255):(hours) )<<24;
		result |= minutes<<16;
		result |= seconds<<8;
		result |= frames;

		return result;
	}

	static byte BCDbyte( byte ip)
	{
		byte up=ip/10;
		byte down=ip%10;

		return (up<<4)+down;
	}

	// Calculates hh:mm:ss:ff as BCD
	// Returns an unsigned int with HMSF as BCD pairs in each byte
	unsigned int Timecode_t::toBCD( bool HoursFirst )
	{
		unsigned int HMSF=toHMSF();

		int result=0;
		char * pHMSF=(char*)( &HMSF );

		if( !HoursFirst )
		{
			result  = BCDbyte(pHMSF[3])<<24;
			result |= BCDbyte(pHMSF[2])<<16;
			result |= BCDbyte(pHMSF[1])<<8;
			result |= BCDbyte(pHMSF[0]);
		}
		else
		{
			result  = BCDbyte(pHMSF[0])<<24;
			result |= BCDbyte(pHMSF[1])<<16;
			result |= BCDbyte(pHMSF[2])<<8;
			result |= BCDbyte(pHMSF[3]);
		}
		return result;
	}

	
	//returns rate as defined in SMPTE326-2000 para 7.2
	byte Timecode_t::GetS326Rate()
	{
		byte result=0;

		switch( _fps )
		{
		case 24: result= 1<<1; break;

		case 25: result= 2<<1; break;

		case 30: result= 3<<1; break;

		default: result=0; break;
		}

		if( _drop)
			result |= 0x01;

		return result;
	}

	// Returns Timecode as defined in SMPTE 331-2004 para 8.2
	unsigned int Timecode_t::toS331DateTime()
	{
		int FSMH=toBCD(false);
		if(_drop)
			FSMH |= 0x40000000;

		return FSMH;
	}

	// cast to string
	Timecode_t::operator char*()
	{
		return GetString();
	}

	// Make a string - with optional specified separator
	char *Timecode_t::GetString(char Sep /*=0*/)
	{
		// only allocate when first used
		if( !_text) _text = new char[16];

		if(Sep == 0)
		{
			switch( _fps )
			{
			case 24: Sep='+'; break;

			case 25: Sep='/'; break;

			case 30: if( _drop) Sep=';'; else Sep=':'; break;

			case 50: Sep='='; break;

			case 60: if( _drop) Sep='~'; else Sep='-'; break;

			case 100: Sep='#'; break;

			default: Sep='?'; break;
			}
		}

		int HMFS=toHMSF();

		unsigned frames =   (HMFS & 0x000000ff);
		unsigned seconds =  (HMFS & 0x0000ff00)>>8;
		unsigned minutes =  (HMFS & 0x00ff0000)>>16;
		unsigned hours =    (HMFS & 0xff000000)>>24;

		sprintf( _text, "%02d%c%02d%c%02d%c%02d", hours,':',minutes,':',seconds,Sep,frames );
		return _text;
	}

	// cast to string
	Timecode_t::operator char const*()
	{
		return operator char*();
	}

