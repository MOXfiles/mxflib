// copyright Metaglue Corp, all rights reserved.

#include "oxstream.h"
using namespace std;

#include <stdarg.h>

#ifndef __WIN32
#include <string.h>
#endif

namespace utility {

// insertion of escaped string
oxstream& operator<<( oxstream& os, const escape& e )
{
	// uncomment this to auto-close open element start
	if( os._open ) { dynamic_cast<std::ostream&>(os) << ">"; os._open=false; }

	// use XML character entity escaping for non-translatable characters
	if( e._c )
	{
		// xml escaping of single character
		if( !isgraph(e._c) && !isspace(e._c) )	(std::ostream &)os << "&#x" << hex << (const int)e._c << ";" ;
		else if( '<'==e._c )	(ostream &)os << "&lt;" ;
		else if( '&'==e._c )	(ostream &)os << "&amp;" ;
		else					(ostream &)os << e._c;
	}
	else
	{
		// xml escaping of string
		std::string::const_iterator s_i = e._s.begin();
		while( s_i != e._s.end() ) os << escape(*s_i++);
	}

	return os;
}

// insertion of const char in utf8
oxstream& operator<<( oxstream& os, const char c )
{ 
	dynamic_cast<std::ostream&>(os) << c; 
	return os; 
}

// insertion of const char* in utf8
oxstream& operator<<( oxstream& os, const char* c )
{ 
	const char* pc = c;
	while( pc && *pc ) os << *pc++;
	return os;
}

// insertion of string in utf8
oxstream& operator<<( oxstream& os, std::string c )
{ 
	std::string::iterator c_i = c.begin();
	while( c_i != c.end() ) os << *c_i++;

	return os;
}

// insertion of wchar_t in utf8
oxstream& operator<<( oxstream& os, const wchar_t c )
{
	char m[6]; // needs to be big enough for longest utf8 rep+1 or "&amp;"

	int nm=wctou8(m, c);

	// use XML character entity escaping for non-translatable characters
	if( (-1)==nm  ) os << "&#x" << hex << (const int)c << ";" ;
	else if( '<'==*m ) os << "&lt;" ;
	else if( '&'==*m ) os << "&amp;" ;
	else { m[nm]='\0'; os << m; }

	return os;
}

// insertion of wchar_t* in utf8
oxstream& operator<<( oxstream& os, const wchar_t* c )
{
	const wchar_t* pc = c;
	while( pc && *pc ) os << *pc++;
	return os;
}

// insertion of wstring in utf8
oxstream& operator<<( oxstream& os, wstring c )
{
	wstring::iterator c_i = c.begin();
	while( c_i != c.end() ) os << *c_i++;

	return os;
}

// insertion of an element
oxstream& operator<<( oxstream& os, const elem& e)
{
	switch( e._ct )
	{
	case 0: // end element
		if( os._open )
		{
			// empty
			os << "/>"; 
			os._open=false;
			--os._in;
			os._n_s.pop();
		}
		else
		{
			// non-simple
			os << --os._in << "</" << os._n_s.top() << ">";
			os._n_s.pop();
		}
		break;

	case 1: // simple content
		if( os._open ) { os << ">"; os._open=false; }
		if( e._c.size() ||  e._isInt )
		{
			os << os._in++ << "<" << e.qname() << ">";
			if( e._isInt)
			{
				char x[32]; 
				if( os.flags() & ios::hex)
					snprintf(x,sizeof(x),"0x%x",e._i); 
				else
					snprintf(x,sizeof(x),"%d",e._i); 
				os << x;
			}
			else
				os << escape( e._c );
			os << "</" << e.qname() << ">";
			--os._in;
		}
		else
		{
			os << os._in << "<" << e.qname() << "/>";
		}
		break;

	case 2: // simple content and end
		if( e._c.size() )
		{
			if( os._open ) { os << ">"; os._open=false; }
			os << escape( e._c );
			os << "</" << os._n_s.top() << ">";
			os._n_s.pop();
			--os._in;
		}
		else
		{
			if( os._open )
			{ 
				os << "/>";
				os._n_s.pop();
				--os._in;
			}
			else
			{
				os << "</" << os._n_s.top() << ">";
				os._n_s.pop();
				--os._in;
			}
		}
		break;

	case 3: // named
		if( os._open ) { os << ">"; os._open=false; }
		os._n_s.push( e.qname() );
		os._open = true;
		os << os._in++ << "<" << os._n_s.top();
		break;
	default:
		os << "!!!BAD ELEMENT CONSTRUCTOR!!!";
		break;
	}
	return os;
}

const ns xmlns( "xmlns", "" );

attr::attr( ns n ) : _ct(1),_n(xmlns),_s(n.tag()),_v(n.nsuri()),_iv(0) {};
   
attr::attr( ns n, const char* s, std::string v ) : _ct(1),_n(n),_s(s),_v(v),_iv(0) {};
   
attr::attr( ns n, const char* s, std::string v, std::string v2 ) : _ct(1),_n(n),_s(s),_v(v),_iv(0) { _v+=" "; _v+=v2; };

attr::attr( ns n, const char* s, std::wstring wv ) : _ct(2),_n(n),_s(s),_v(""),_iv(0),_wv(wv) {};
   
attr::attr( ns n, const char* s, const int v ) : _ct(3),_n(n),_s(s),_v(""),_iv(v) {};

attr::attr( ns n, const char* s, std::string v, std::wstring wv2 ) : _ct(4),_n(n),_s(s),_v(v),_iv(0),_wv(wv2) {};


oxstream& operator<<(oxstream& os, const attr & att)
{
	 switch(att._ct)
	 {
	 case 1:
		 if( 0!=att._s.size() )
		 {

			 os << " " << att._n.qual();
			 os << att._s << "=\"" << att._v << "\"";
		 }
		 else
		 {
			 // deal with xmlns for untagged namespace
			 os << " " << att._n.tag() << "=\"" << att._v << "\"";
		 }
		 break;
	 case 2:
		 if( 0!=att._s.size() )
		 {
			 os << " " << att._n.qual();
			 os << att._s << "=\"" << att._wv << "\"";
		 }
		 break;
	 case 3:
		 if( 0!=att._s.size() )
		 {
			 os << " " << att._n.qual();
			 os << att._s << "=\"" << dec << (const int)att._iv << hex << "\"";
		 }
		 break;
	 case 4:
		 if( 0!=att._s.size() )
		 {
			 os << " " << att._n.qual();
			 if( 0!=att._v.size() ) os << att._v << " ";
			 os << att._s << "=\"" << att._wv << "\"";
		 }
		 break;

	 default:
		 os<<"Bad Constructor type in attr::";
		 break;
	 }
	 return os;
};



} // namespace utility

