// copyright Metaglue Corp, all rights reserved.

#include "osprintf.h"

namespace utility {

void osprintf::build_osprintf( const char *fmt, va_list args )
{
	while( next_sz < BIGGESTSIZE ) // run away from runaway
	{
		if( buf ) delete[] buf;
		buf = new char[next_sz];
		sz = next_sz;

		// thanks to http://perfec.to/vsnprintf/ for portable interpretation of vsnprintf
		int outsize = vsnprintf( buf, sz, fmt, args );

		if( outsize == -1 )
		{
			// Clear indication that output was truncated
			// No clear indication of how big buffer needs to be
			// Double existing buffer size for next time.
			next_sz = sz * 2;
		} 
		else if( outsize == sz )
		{
			// Output was truncated (since at least the \0 could not fit)
			// No indication of how big the buffer needs to be
			// Double existing buffer size for next time.
			next_sz = sz * 2;
		}
		else if( outsize > (int) sz )
		{
			// Output was truncated and we were told exactly how big the buffer needs to be next time
			// Add two chars to the returned size. One for the \0, and one to prevent ambiguity in the next case below.
			next_sz = outsize + 2;
		} 
		else if( outsize == sz - 1 )
		{
			// Ambiguous. May mean that the output string exactly fits
			// But on some implementations the output string may have been truncated
			// Double the buffer size for next time.
			next_sz = sz * 2;
		} 
		else
		{
			/* Output was not truncated */
			buf[outsize] = '\0';			// force a null terminator just in case
			break;
		}
	}
};

osprintf::osprintf( const char *fmt, va_list args ) : buf(NULL),sz(0),next_sz(FIRSTSIZE)
{
	build_osprintf(fmt, args);
};

osprintf::osprintf( const char *fmt, ... ) : buf(NULL),sz(0),next_sz(FIRSTSIZE)
{
	va_list args;
	va_start(args, fmt);
	build_osprintf(fmt, args);
	va_end(args);
};

} // namespace utility

