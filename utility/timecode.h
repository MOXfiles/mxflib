/*! \file	timecode.h
 *	\brief	Timecode handling class for MXF 
 *
 *	\version $Id$
 *
 */
/*
 *	Copyright (c) 2012, Metaglue Corporation. All rights reserved.
 */


#ifndef _TIMECODE_H_
#define _TIMECODE_H_

#include <string>

// to reconcile libprocesswrap with M3BaseLib
//#define EXCLUDE_MXFLIB_H

#ifndef EXCLUDE_MXFLIB_H

#include "mxflib/mxflib.h"
//using namespace mxflib;
// Ensure we add namespace to types defined by mxflib

#ifndef MXFLIB_TYPE
#define MXFLIB_TYPE(x) mxflib::x
#endif // MXFLIB_TYPE

#else

// Position is declared in mxflib.h
#include "platform.h"
typedef Int64 Position;
// Use local types rather than mxflib ones

#ifndef MXFLIB_TYPE
#define MXFLIB_TYPE(x) x
#endif // MXFLIB_TYPE

#endif

typedef unsigned char byte;




const unsigned defaultfps = 1000;

class Timecode_t
{
private:
	MXFLIB_TYPE(Position) _start;
	unsigned	_fps;
	bool		_drop;
	bool		_valid;		// true if an explicit _start and _fps has been specified

	char*		_text;		// to hold the canonical text form for operator char*

public:
	// destructor
	~Timecode_t() { if( _text ) delete [] _text; }

	// construct from string
	// note that defaults to 1000 fps
	// MUST call SetFPS() before persisting
	Timecode_t( const char* arg, unsigned fps = defaultfps, bool drop = false );

	// construct from  unsigned, fps and drop flag  (and default)
	// note that defaults to 1000 fps - MUST call SetFPS() before persisting
	Timecode_t( MXFLIB_TYPE(Position) arg = 0, const unsigned fps = defaultfps, const bool drop = false );

	// Set the frames per second indicator and update the framecount accordingly
	// do NOT affect _valid
	void SetFPS( unsigned fps, bool drop=false );

	// Set to a frame count
	// does NOT affect _drop
	void Set( MXFLIB_TYPE(Position) start, unsigned fps = defaultfps );

	//! Set the value from a string, optionally inferring the FPS and drop (return true if OK)
	bool Set( const char *arg, bool InferFPS = false);

	//! Set from Hours, Minutes, Seconds, Frames
	void Set( int Hours, int Minutes, int Seconds, int Frames);

	// Get the frames per second indicator
	unsigned GetFPS( ) const { return _fps; };

	// Is it the dreaded Drop Frame?
	bool isDrop() const { return _drop; }

	// has fps been set?
	bool isValid() const { return _valid && _fps != defaultfps; }

	// copy constructor
	Timecode_t(const Timecode_t& s) { _start=s._start; _valid=s._valid; _fps=s._fps; _drop=s._drop; _text=NULL; }

	// assignment
	Timecode_t& operator=(const Timecode_t& s) { _start=s._start; _valid=s._valid; _fps=s._fps; _drop=s._drop; _text=NULL; return *this; }

	// test equality
	bool operator==( const Timecode_t &s ) { return _valid && s._valid && _start==s._start && _fps==s._fps && _drop==s._drop; }

	//add number of frames
	Timecode_t&  operator+( int nFrames) { _start+=nFrames; return *this; };
	Timecode_t&  operator-( int nFrames) { _start-=nFrames; return *this; };
	Timecode_t&  operator+=( int nFrames) { _start+=nFrames; return *this; };
	Timecode_t&  operator-=( int nFrames) { _start-=nFrames; return *this; };

	// return 4 BCD encoded bytes
	// returns as an unsigned int with the HMSF as BCD pairs in each byte
	unsigned int toBCD( bool HoursFirst=true);

	// return rate as defined in SMPTE326-2000 para 7.2
	byte GetS326Rate();

	// return Timecode as defined in SMPTE 331-2004 para 8.2
	unsigned int toS331DateTime();

	// intermediate function that calculates H:M:S:F
	// returns as an unsigned int with the HMSF as unsigned int in each byte
	unsigned int toHMSF();

	// add, subtract a number of frames
	// do NOT affect _valid
	Timecode_t operator+(const MXFLIB_TYPE(Position) n) { Timecode_t r(*this); r._start+=(unsigned int)n; return r; }
	Timecode_t operator-(const MXFLIB_TYPE(Position) n) { Timecode_t r(*this); r._start-=(unsigned int)n; return r; }

	Timecode_t & operator++()      //prefix operator
	{
		_start++;
		return *this;
	}

	Timecode_t &  operator++(int)  //postfix operator
	{
		_start++;
		return *this;
	}

	// get frame count
	MXFLIB_TYPE(Position) Start() const { return _start; } // even if !_valid

	// cast to string
	operator char*();
	operator char const*();
	char const *GetString() { return operator char const *(); }
};

#endif //_TIMECODE_H_

