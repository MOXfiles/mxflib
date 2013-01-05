// copyright Metaglue Corp, all rights reserved.

#ifndef __osprintf_h__
#define __osprintf_h__

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include <cstring>
#include <string>
#include <iostream>
using namespace std;

#ifdef _MSC_VER
#ifndef vsnprintf
#define vsnprintf _vsnprintf
#endif
#endif

namespace utility {

// portable c++ implementation of sprintf for use as ostream manipulator
class osprintf
{
private:
	char*	buf;
	size_t	sz;
	size_t	next_sz;

	// size of first buffer; start small to exercise grow routines
	// but in 2011, memory is cheap
	static const size_t FIRSTSIZE = 256;

	// size of biggest buffer; you can make this as big as you dare
	static const size_t BIGGESTSIZE = 65536;

	// internal builder
	void build_osprintf( const char *fmt, va_list args );

public:
	// usual constructor - like sprintf
	osprintf( const char *fmt, va_list args );

	// usual constructor - like sprintf
	osprintf( const char *fmt, ... );

	// destructor
	~osprintf(){ if( buf ) delete[] buf; }

	// copy constructor
	osprintf(const osprintf &s) { sz=s.sz; next_sz=s.next_sz; buf=new char[sz]; strncpy(buf,s.buf,sz); }

	// assignment
	osprintf& operator=(const osprintf &s) { sz=s.sz; next_sz=s.next_sz; buf=new char[sz]; strncpy(buf,s.buf,sz); return *this; }

	// test equality
	bool operator==( const osprintf &s ) { return 0==strncmp( buf,s.buf,(sz<s.sz)?sz:s.sz ); }

	// cast to cstring
	operator const char *() const { return buf; }

	// cast to std::string
	operator std::string() const { return buf; } 

	// insertion in ostream
	friend ostream& operator<<( ostream& os, const osprintf& s );
};

// insertion of sprintf-like cstring
inline ostream& operator<<( ostream& os, const osprintf& s ){ os << s.buf; return os; }

} // namespace utility

#endif // __osprintf_h__



