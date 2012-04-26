// copyright Metaglue Corp, all rights reserved.

#ifndef __oxstream_h__
#define __oxstream_h__

#include "utf8.h"

#include <limits.h>

#include <wchar.h>

#include <stack>
#include <string>
#include <iostream>
#include <fstream>
#include <strstream>



#ifdef _MSC_VER
#ifndef snprintf
#define snprintf _snprintf
#endif
#endif

#define ENDL "\n"

namespace utility {

class oxstream;
class attr;

// indent counter
class indented
{
private:
	int _indents;
public:
	indented() : _indents(0) {};
	indented( const int n ) : _indents(n) {};

	/* prefix ++ */	 indented& operator++()   { ++_indents; return *this; }
	/* prefix -- */	 indented& operator--()   { if( _indents) --_indents; return *this; }
	/* postfix ++ */ indented operator++(int) { indented i = *this; ++*this; return i; }
	/* postfix -- */ indented operator--(int) { indented i = *this; --*this; return i; }

	friend oxstream & operator<<( oxstream& os, const indented& indent );
};

// insertion of indent
#define INDENT "  "
inline oxstream& operator<<( oxstream& os, const indented& indent )
{
	int indents= indent._indents;
	(std::ostream &)os << ENDL;
	while( indents-- ) (std::ostream &)os << INDENT;
	return os;
}  


// output xml file stream
class oxstream : public std::ostream
{
public:
	indented _in;				// number of indents from left margin
	bool _open;					// true = start element still open
	std::stack<std::string> _n_s;	// qname of start element

	oxstream( std::basic_streambuf<char> *_rdbuf ) : std::ostream(_rdbuf),_in(0),_open(false) 
	{

	};
	oxstream(  ) : std::ostream(std::cout.rdbuf()),_in(0),_open(false) {};

	friend oxstream & operator<<( oxstream& os, const char c );
	friend oxstream & operator<<( oxstream& os, const char* c );
	friend oxstream & operator<<( oxstream& os, std::string c );

private:
	std::ofstream* pOsf;
};

// insertion of const char in utf8
oxstream& operator<<( oxstream& os, const char c );

// insertion of const char* in utf8
oxstream& operator<<( oxstream& os, const char* c );

// insertion of string in utf8
oxstream& operator<<( oxstream& os, std::string c );

// insertion of wchar_t in utf8
oxstream& operator<<( oxstream& os, const wchar_t c );

// insertion of wchar_t*
oxstream& operator<<( oxstream& os, const wchar_t* c );

// insertion of wstring
oxstream& operator<<( oxstream& os, std::wstring c );



//! ns is a definition of an XML namespace declaration
class ns 
{ 
private:
	std::string _tag;
	std::string _nsuri;
	std::string _qual;

public:
	ns() {};
	ns( const char *tag, const char *nsuri ) : _tag(tag), _nsuri(nsuri), _qual(tag) { if( _tag.size() ) _qual += ":"; }

	std::string tag() const { return _tag; }
	std::string qual() const { return _qual; }
	std::string nsuri() const { return _nsuri; }
};

// xml-escaped simpleType content
class escape
{
private:
	const char _c;
	std::string _s ;
public:
	escape() : _c(0) {};
	escape( const char c ) : _c(c) {};
	escape( const char* s ) : _c(0),_s(s) {};
	escape( std::string s ) : _c(0),_s(s) {};
	friend oxstream & operator<<( oxstream& os, const escape& e );
};

// insertion of escape() in utf8
oxstream& operator<<( oxstream& os, const escape& c );


class oxfstream : public oxstream
{
public:

	oxfstream( const char * filename): oxstream(fstream.rdbuf())
	{
		fstream.open(filename);

	}

	void close()
	{
		fstream.close();
	}

	bool is_open() const
	{
		return fstream.is_open();
	}

#ifdef _WIN32
	void open(
		const char *_Filename,
		ios_base::openmode _Mode = ios_base::out,
		int _Prot = (int)ios_base::_Openprot
		)
	{
		fstream.open(_Filename,_Mode,_Prot);
	}

#else
	
	void open(
		const char *_Filename,
		ios_base::openmode _Mode = ios_base::out
		)
	{
		return fstream.open(_Filename,_Mode);
	}
#endif

	std::basic_filebuf<char> *rdbuf( ) const
	{
		return fstream.rdbuf();
	}
protected:
	std::ofstream  fstream;


};



class oxstrstream : public oxstream
{
public:

	oxstrstream( ): oxstream(strstream.rdbuf())
	{
	}

	void freeze( bool _Freezeit = true)
	{
		return strstream.freeze(_Freezeit);
	}


	std::strstreambuf *rdbuf( ) const
	{
		return strstream.rdbuf();
	}

	std::streamsize pcount( ) const
	{
		return strstream.pcount();
	}

	char *str( )
	{
		return strstream.str();
	}

protected:
	std::ostrstream  strstream;


};




// elements
class elem
{
public:
	elem()		   : _ct(0),_isInt(false),_i(0) {};								// end element

	elem( int ct ) : _ct(ct),_isInt(false),_i(0) {};							// marker element 0 = empty, 1 = nonempty

	elem( int ct, std::string c ) : _ct(ct),_c(c),_isInt(false),_i(0) {};							// 0 = empty, 1 = nonempty

	elem( ns n, std::string s, std::string c ) : _ct(1),_n(n),_s(s),_c(c),_isInt(false),_i(0) {};  // 1 = nonempty
	elem( ns n, std::string s, const int i ) : _ct(1),_n(n),_s(s),_isInt(true),_i(i) { };

	elem( ns n, std::string s ) : _ct(3),_n(n),_s(s) {};	// 3 = named element

	std::string qname() const { return _n.qual() + _s; }

	friend oxstream& operator<<(oxstream& s, const elem &e );
private:
	int _ct;

	bool _isInt;
	int  _i;

	ns _n ;
	std::string _s ;
	std::string _c ;
};

//#define empty() elem(0)
#define simple(A) elem(2,A)


// attributes
class attr
{
public:
	attr() : _ct(0),_n(),_s(""),_iv(0) {};
	attr( ns n );
	attr( ns n, const char* s, std::string v );
	attr( ns n, const char* s, std::string v, std::string v2 );
	attr( ns n, const char* s, std::wstring wv );
	attr( ns n, const char* s, const int v);
	attr( ns n, const char* s, std::string v, std::wstring v2 );

	std::string qname() const { return _n.qual() + _s; }

	friend oxstream& operator<<(oxstream& s, const attr& att );
private:
	int			_ct;

	ns			_n ;
	std::string _s ;
	std::string _v;
	const int	_iv ;
	std::wstring _wv;
};




} // namespace utility

#endif // __oxstream_h__



