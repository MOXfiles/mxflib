
//#define to make sure that CrashRecoveryInst is not instantiated in this file. 
#define CRASH_IMPLEMENTATION
#include "CrashRecovery.h"

#ifdef _WIN32

#include <Windows.h>
#include <stdio.h>
#include <string>
#include <DbgHelp.h>


FILE * sgLogFile;

#pragma comment(lib,"Ws2_32.lib")
#pragma comment(lib,"version.lib")
#pragma comment(lib,"DbgHelp.lib")

namespace utility {

static const char * from_id="support@metaglue.com";


struct _SMTPServers
{
	const char * mailserver;
	unsigned short port;
	const char * to_id;

};

struct _SMTPServers SMTPServers[]={
#ifdef _DEBUG
	{"mail.metaglue.net", 25, "ian@metaglue.net" },
#endif
	{"mail.metaglue.com", 25, "crash.report@metaglue.com" },
	{"report.metaglue.net", 80, "crash.report@metaglue.net" },
	{NULL, 25 }
};

static const std::string base64_chars = 
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";


void base64_encode( std::string & ret, char const* bytes_to_encode, unsigned int in_len) 
{
	int i = 0;
	int j = 0;
	unsigned char char_array_3[3];
	unsigned char char_array_4[4];

	while (in_len--) {
		char_array_3[i++] = (unsigned char)*(bytes_to_encode++);
		if (i == 3) {
			char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
			char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
			char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
			char_array_4[3] = char_array_3[2] & 0x3f;

			for(i = 0; (i <4) ; i++)
				ret += base64_chars[char_array_4[i]];
			i = 0;
		}
	}

	if (i)
	{
		for(j = i; j < 3; j++)
			char_array_3[j] = '\0';

		char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
		char_array_4[1] = ((char_array_3[0] & 0x03) << 4) + ((char_array_3[1] & 0xf0) >> 4);
		char_array_4[2] = ((char_array_3[1] & 0x0f) << 2) + ((char_array_3[2] & 0xc0) >> 6);
		char_array_4[3] = char_array_3[2] & 0x3f;

		for (j = 0; (j < i + 1); j++)
			ret += base64_chars[char_array_4[j]];

		while((i++ < 3))
			ret += '=';
	}
}


static bool SendPermitted()
{
	DWORD KeyVal=0;
	DWORD KeyValLen=sizeof (KeyVal);
	DWORD Type=REG_DWORD;

	HKEY MGKey;

	DWORD regResult=RegOpenKeyEx(HKEY_CURRENT_USER,"Software\\Metaglue", 0, KEY_READ, &MGKey);

	if(regResult==0)
	{
		regResult=::RegQueryValueEx(
			MGKey,"SendCrashReport",NULL,
			&Type ,
			(LPBYTE) &KeyVal, &KeyValLen);
		
		if(regResult==0 )
			RegCloseKey( MGKey );
	}

	return  KeyVal!=0;
}


static int send_socket(int sock, const char *s)
{
	int result=send(sock,s,(int)strlen(s),0);
	if(SOCKET_ERROR ==result)
	{
		return -1;
	}

	return 0;
}


static int read_socket( int sock, char * buf, int szBuff)
{
	buf[0]='\0';
	int len = recv(sock,buf,szBuff,0);
	if(SOCKET_ERROR ==len)
	{
		return -1;
	}
	buf[len]='\0';

	return len;
}

static bool ack( int sock, const char* r )
{
	char buf[BUFSIZ+1];
	int len = read_socket(sock,buf,BUFSIZ);

	if( len<strlen(r) || 0!=strncmp( buf, r, strlen(r) ) ) return false;

	return true;
}

bool  SendEmail( const char * subject, const char * message, const char * DumpFileName)
{
	WSADATA wsaData;
	int err;

	WORD wVersionRequested = MAKEWORD( 2, 2 );

	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		/* Tell the user that we could not find a usable */
		/* WinSock DLL.                                  */
		return false;
	}

	// Get local host name
	const char * szHostName=utility::CrashRecoveryInst.GetDNSHostname();
	if(szHostName==NULL)
		return false;

	struct sockaddr_in SvrAddr;
	struct hostent *hp;
	unsigned short     ServerPort=25;

	SOCKET sock=socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

	memset(&SvrAddr,0,sizeof(SvrAddr));
	SvrAddr.sin_family=AF_INET;
	SvrAddr.sin_addr.s_addr=htonl( INADDR_ANY);
	SvrAddr.sin_family = AF_INET;

	int index=0;
	while( SMTPServers[index].mailserver!=NULL)
	{
		hp = gethostbyname(  SMTPServers[index].mailserver );
		SvrAddr.sin_port=htons( SMTPServers[index].port);
		if (hp!=(struct hostent *) 0)
		{
			if( bind( sock, (struct sockaddr* )& SvrAddr, sizeof(SvrAddr) )<0)
				break;
		}
	}

	if( SMTPServers[index].mailserver==NULL)
	{ //have not successfully connected ot one of hte servers
		WSACleanup();
		return false;
	}

	int  timeout=5000;  //5 seconds
	setsockopt( sock, SOL_SOCKET, SO_RCVTIMEO,(const char*) &timeout, sizeof(timeout) );

	memcpy((char *) &SvrAddr.sin_addr, (char *) hp->h_addr, hp->h_length);

	if (connect(sock, (struct sockaddr *) &SvrAddr, sizeof SvrAddr)==-1)
	{
		WSACleanup();
		return false;
	}

	char buf[BUFSIZ+1];
	std::string msg;

	read_socket(sock ,buf,BUFSIZ ); /* SMTP Server logon string */

	// basic hello
	msg="HELO ";
	msg+=szHostName;
	msg+="\r\n";
	send_socket(sock , msg.c_str() ); 
	if( !ack(sock ,"250") ) 
	{
		closesocket(sock);
		WSACleanup();
		return false;
	}

	msg="MAIL FROM:<"; 
	msg+=from_id;
	msg+=">\r\n";
	send_socket(sock , msg.c_str() ); 
	if( !ack(sock ,"250") ) 
	{
		closesocket(sock);
		WSACleanup();
		return false;
	}

	// TODO divide up multiple addressees
	msg="RCPT TO:<"; /*Mail to*/
	msg+=SMTPServers[index].to_id;
	msg+=">\r\n";
	send_socket(sock , msg.c_str() ); 
	if( !ack(sock ,"25") )
	{
		closesocket(sock);
		WSACleanup();
		return false;
	}

	msg="DATA\r\n";// body to follow*/
	msg+="From: ";

	msg+="\"";
	msg+=from_id;
	msg+="\" ";

	msg+="<";
	msg+=from_id;
	msg+=">\r\n";

	msg+="Reply-To: ";
	msg+=from_id;
	msg+="\r\n";

	msg+="To: ";
	msg+=SMTPServers[index].to_id;
	msg+="\r\n";

	msg+="Subject: ";
	msg+=subject;
	msg+="\r\n";

	msg+="MIME-Version: 1.0\r\nContent-Type: multipart/mixed; boundary=frontier\r\n";
	msg+="\r\n\r\n";
	send_socket(sock , msg.c_str() ); 
	if( !ack(sock ,"354") )
	{
		closesocket(sock);
		WSACleanup();
		return false;
	}

	send_socket(sock ,"--frontier\r\nContent-Type: text/plain\r\n\r\n");
	// TODO expand lines that consist only of '.' into '..'
	send_socket(sock ,message);

	if(DumpFileName!=NULL)
	{		
		FILE * IP=fopen(DumpFileName,"rb");
		if(IP)
		{
			msg="\r\n--frontier\r\nContent-Type: application/octet-stream\r\nContent-Transfer-Encoding: base64\r\n";
			msg+="Content-Disposition: attachment; filename=crash.mdmp;\r\n\r\n";

			fseek(IP,0L,SEEK_END);
			size_t DumpLen=ftell(IP);
			fseek(IP,0L,SEEK_SET);
			char * buff=new char[DumpLen];
			size_t nread=fread( buff,1,DumpLen, IP );
			fclose(IP);
			base64_encode(msg,buff,(int)DumpLen);
			delete[] buff;
		}
	}

	msg+="\r\n--frontier--\r\n.\r\n";
	send_socket(sock , msg.c_str() ); 
	if( !ack(sock ,"250") )
	{
		closesocket(sock);
		WSACleanup();
		return false;
	}

	send_socket(sock ,"QUIT\r\n"); /* quit */
	read_socket(sock ,buf,BUFSIZ); // log off */
	if( !ack(sock ,"") )
	{
		closesocket(sock);
		WSACleanup();
		return false;
	}

	closesocket(sock);
	WSACleanup();

	return true;
}


} //namespace utility 



typedef void (WINAPI *WINDOWS_PGNSI)(LPSYSTEM_INFO);
static char PathToExecutable[MAX_PATH+1];
static  bool CalledHandler=false; //make sure we only call the handler once


#ifndef _M_X64
void   LogStackFrames(std::string & strExcepData, PVOID FaultAddress, char *eNextBP)
{  
	char strWorkspace[100];
	char *p, *pBP;                                    
	unsigned i, x, BpPassed;
	static int  CurrentlyInTheStackDump = 0;

	BpPassed = (eNextBP != NULL);
	if(! eNextBP)
	{
		_asm mov     eNextBP, eBp  
	}
	else
	{
		sprintf(strWorkspace, "\n  Fault Occurred At $ADDRESS:%08LX\n", (int)FaultAddress);
		strExcepData+=strWorkspace;
	}
	// prevent infinite loops
	for(i = 0; eNextBP && i < 100; i++)
	{     
		pBP = eNextBP;           // keep current BasePointer
		eNextBP = *(char **)pBP; // dereference next BP
		p = pBP + 8;
		// Write 20 Bytes of potential arguments
		strExcepData+= "         with ";       
		for(x = 0; p < eNextBP && x < 20; p++, x++)
		{
			sprintf(strWorkspace, "%02X ", *(unsigned char *)p);
			strExcepData+=strWorkspace;
		}

		strExcepData+= "\n\n";         
		if(i == 1 && ! BpPassed)
			strExcepData+="*************************************\n         Fault Occurred Here:\n";

		// Write the backjump address
		sprintf(strWorkspace, "*** %2d called from $ADDRESS:%08LX\n", i, *(char **)(pBP + 4));
		strExcepData+=strWorkspace;
		if(*(char **)(pBP + 4) == NULL)
			break;
	}
}
#endif


void   GetSystemInfo(std::string& Ret)
{

	Ret = "Windows";

	OSVERSIONINFOEX OSInfo;

	// Flag that we want the full info (if possible)
	OSInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEX);

	BOOL Result = GetVersionEx((OSVERSIONINFO*)&OSInfo);
	if(!Result)
	{
		// Try the old version (we may be on an old version of Windows)
		OSInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
		Result = GetVersionEx((OSVERSIONINFO*)&OSInfo);
	}

	if(Result)
	{
		if(OSInfo.dwMajorVersion == 4)
		{
			if(OSInfo.dwPlatformId & VER_PLATFORM_WIN32_NT) Ret = "Windows NT";
			else Ret = "Windows 95";
		}
		else if(OSInfo.dwMajorVersion == 5)
		{
			if(OSInfo.dwMinorVersion == 0) Ret = "Windows 2000";
			else if(OSInfo.dwMinorVersion == 1) Ret = "Windows XP";
			else if(OSInfo.dwMinorVersion == 2) 
			{
				if(OSInfo.wProductType & VER_NT_SERVER) 
					Ret = "Windows Server 2003";
				else 
					Ret = "Windows XP x64";
			}
			else if(OSInfo.dwMinorVersion == 10) Ret = "Windows 98";
			else if(OSInfo.dwMinorVersion == 90) Ret = "Windows Me";
		}
		else if(OSInfo.dwMajorVersion == 6)
		{
			if(OSInfo.dwMinorVersion == 0) 
			{
				if(OSInfo.wProductType != VER_NT_WORKSTATION) 
					Ret = "Windows Server 2008";
				else 
					Ret = "Windows Vista";
			}
			else if(OSInfo.dwMinorVersion == 1)
			{
				if(OSInfo.wProductType != VER_NT_WORKSTATION) 
					Ret = "Windows Server 2008 R2";
				else 
					Ret = "Windows 7";
			}
		}

		// Add any service pack details
		if(OSInfo.szCSDVersion[0])
		{
			if(OSInfo.dwPlatformId & VER_PLATFORM_WIN32_WINDOWS)
			{
				if(OSInfo.szCSDVersion[1] == 'A') Ret += " Second Edition";
				else if(OSInfo.szCSDVersion[1] == 'B') Ret += " Second Edition";
				else if(OSInfo.szCSDVersion[1] == 'C') Ret += " OSR2";
			}
			else Ret += " " + std::string(OSInfo.szCSDVersion);
		}
	}

	if ( OSInfo.dwMajorVersion >= 5 )
	{
		// Read system info from Windows kernel
		SYSTEM_INFO si;
		WINDOWS_PGNSI pGNSI = (WINDOWS_PGNSI) GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "GetNativeSystemInfo");
		if(pGNSI) pGNSI(&si);
		else GetSystemInfo(&si);

		if ( si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_AMD64 )
			Ret += " (64-bit)";

		else if (si.wProcessorArchitecture == PROCESSOR_ARCHITECTURE_IA64 )
			Ret += " (Itanium)";

	}
}



long Win32FaultHandler( struct _EXCEPTION_POINTERS *  ExInfo)
{ 
	if(CalledHandler)
		return EXCEPTION_EXECUTE_HANDLER;

	if(!utility::SendPermitted())
	{
		exit(0);
		return EXCEPTION_EXECUTE_HANDLER;
	}

	DWORD ExcpCode=ExInfo->ExceptionRecord->ExceptionCode;

	if( ExcpCode==EXCEPTION_FLT_INEXACT_RESULT || ExcpCode==EXCEPTION_FLT_OVERFLOW ||
		ExcpCode==EXCEPTION_FLT_DENORMAL_OPERAND || ExcpCode==EXCEPTION_FLT_UNDERFLOW
		|| ExcpCode==EXCEPTION_INT_OVERFLOW )
		return EXCEPTION_EXECUTE_HANDLER;

	CalledHandler=true;

	char strWorkspace[100];
	std::string strExcepData;

	char  *FaultTx = "";
	switch(ExInfo->ExceptionRecord->ExceptionCode)
	{
	case EXCEPTION_ACCESS_VIOLATION         : FaultTx = "ACCESS VIOLATION"         ; break;
	case EXCEPTION_DATATYPE_MISALIGNMENT    : FaultTx = "DATATYPE MISALIGNMENT"    ; break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO       : FaultTx = "INT DIVIDE BY ZERO"       ; break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO       : FaultTx = "FLT DIVIDE BY ZERO"       ; break;
	case EXCEPTION_STACK_OVERFLOW           : FaultTx = "STACK OVERFLOW"           ; break;
	case EXCEPTION_FLT_INVALID_OPERATION    : FaultTx = "FLT INVALID OPERATION"    ; break;
	case EXCEPTION_ILLEGAL_INSTRUCTION      : FaultTx = "ILLEGAL INSTRUCTION"      ; break;
	case EXCEPTION_IN_PAGE_ERROR            : FaultTx = "IN PAGE ERROR"            ; break;
	case EXCEPTION_INVALID_DISPOSITION      : FaultTx = "INVALID DISPOSITION"      ; break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION : FaultTx = "NONCONTINUABLE EXCEPTION" ; break;

	default: FaultTx = "(unknown)";           break;
	}

	int    wsFault    = ExInfo->ExceptionRecord->ExceptionCode;
	PVOID  CodeAddress = ExInfo->ExceptionRecord->ExceptionAddress;

	strExcepData+="****************************************************\n";
	strExcepData+="*** A Program Fault occurred:\n";
	sprintf(strWorkspace, "*** Error code %08X: %s\n", wsFault, FaultTx);
	strExcepData+=strWorkspace;
	strExcepData+= "****************************************************\n";
#ifdef _M_X64
	sprintf(strWorkspace, "***   Address: %08llX\n", (unsigned __int64)CodeAddress);
#else
	sprintf(strWorkspace, "***   Address: %08X\n", (int)CodeAddress);
#endif
	strExcepData+=strWorkspace;
	sprintf(strWorkspace, "***     Flags: %08X\n",ExInfo->ExceptionRecord->ExceptionFlags);
	strExcepData+=strWorkspace;

	try{
		strExcepData="\nMetaglue Exception in ";
		strExcepData+=PathToExecutable;
		strExcepData+="\n";
		strExcepData+="Hostname:";
		strExcepData+=utility::CrashRecoveryInst.GetDNSHostname();
		strExcepData+="\n";

		strExcepData+="Operating System: ";
		std::string OStype;
		GetSystemInfo(OStype);
		strExcepData+=OStype;
		strExcepData+="\n";

		SYSTEMTIME systm;
		GetLocalTime(&systm);
		strExcepData+="Time: ";
		sprintf(strWorkspace, "%d-%d-%d  %02d:%02d:%02d.%03d\n", systm.wYear, systm.wMonth, systm.wDay, 
			systm.wHour,systm.wMinute,systm.wMinute,systm.wMilliseconds);

		strExcepData+=strWorkspace;

		TIME_ZONE_INFORMATION TimeZoneInformation;
		DWORD result=GetTimeZoneInformation(&TimeZoneInformation);
		if(result!=TIME_ZONE_ID_INVALID)
		{
			char tmp[32];
			int hrs;
			if(result==TIME_ZONE_ID_UNKNOWN || result==TIME_ZONE_ID_STANDARD)
			{
				wcstombs(tmp,TimeZoneInformation.StandardName,32);
				hrs=TimeZoneInformation.StandardBias/60;
			}
			else
			{
				wcstombs(tmp,TimeZoneInformation.DaylightName,32);
				hrs=TimeZoneInformation.DaylightBias/60;
			}
			strExcepData+="Workstation Timezone: ";
			strExcepData+=tmp;
			strExcepData+=" (";
			_ltoa(hrs,tmp,10);
			strExcepData+=tmp;
			strExcepData+=")\n";
		}

		/************************************************************************
		***
		*** Get Version Info   
		***
		************************************************************************/

		const char * version="Unknown";

		char pNameBuff[MAX_PATH+1];
		void* pVerBuff;
		DWORD nNameBuffSize = MAX_PATH;
		DWORD nSize = GetModuleFileName( GetModuleHandle("WinStop.exe"),(LPTSTR)pNameBuff, nNameBuffSize);
		if(nSize > 0)
		{
			// now use the location to open the resource
			DWORD zero;
			
			DWORD fiSize = ::GetFileVersionInfoSize( (LPTSTR)pNameBuff, &zero);
			if(fiSize> 0)
			{
				char* pInfo = new char[fiSize+1];
				BOOL bGood = ::GetFileVersionInfo( (LPTSTR)pNameBuff, 0, fiSize+1, pInfo);
				if(bGood)
				{
					
					UINT pSize;

					BOOL bVer = VerQueryValue(pInfo, "\\VarFileInfo\\Translation", &pVerBuff, &pSize );

					char versionLocation[200];
					byte * LangCodepage= reinterpret_cast<byte*>(pVerBuff);
					sprintf(versionLocation,"\\StringFileInfo\\%02x%02x%02x%02x\\ProductVersion",LangCodepage[1],LangCodepage[0],LangCodepage[3],LangCodepage[2]);

					bVer = VerQueryValue(pInfo, versionLocation, &pVerBuff, &pSize );

					if(bVer && pSize > 0)
						version = (LPCTSTR)pVerBuff;
				}
				delete[] pInfo;
			}
		}

		strExcepData+="App Version: ";
		strExcepData+=version;
		strExcepData+="\n";

		strExcepData+="Build Machine: ";
		strExcepData+=utility::__CrashImpl_ImplMachine;
		strExcepData+="\n";

		strExcepData+="Instantiated in: ";
		strExcepData+=utility::__CrashImpl_ImplFile;
		strExcepData+="\n";

		strExcepData+="Build Time: ";
		strExcepData+=utility::__CrashImpl_BuildTime;
		strExcepData+="\n\n\n";




		/************************************************************************
		***
		*** Look for Full Drives   
		***
		************************************************************************/

		ULARGE_INTEGER FreeBytesAvailable;
		ULARGE_INTEGER TotalNumberOfBytes;
		ULARGE_INTEGER TotalNumberOfFreeBytes;

		DWORD d = GetLogicalDrives();
		int i;
		char Drive[] = "A:\\";
		for(i=0;i<26;i++)
		{
			if(d & (1<<i))
			{
				Drive[0] = 'A'+i;
				if(GetDiskFreeSpaceEx( Drive, &FreeBytesAvailable,&TotalNumberOfBytes,&TotalNumberOfFreeBytes))
				{
					UINT DrvType=GetDriveType(Drive);
					const char * strDrvType;
					switch(DrvType)
					{
					case DRIVE_NO_ROOT_DIR: strDrvType="Invalid"; break;
					case DRIVE_REMOVABLE: strDrvType="Removable"; break;
					case DRIVE_FIXED: strDrvType="Fixed"; break;
					case DRIVE_REMOTE: strDrvType="Network"; break;
					case DRIVE_CDROM: strDrvType="CDROM"; break;
					case DRIVE_RAMDISK: strDrvType="RAM Disk"; break;
					case DRIVE_UNKNOWN:
					default: strDrvType="Type Unknown"; break;
					}
					sprintf(strWorkspace, "Drive %s: %-10s %lld/%lld\n",Drive,strDrvType,FreeBytesAvailable.QuadPart,TotalNumberOfBytes.QuadPart);
					strExcepData+=strWorkspace;
				}
				
			}
		}

	}
	catch(...)
	{
		//just in case we make a mistake in the handler
	}

	fflush(stdout); //make sure that everything is output before we do the complex stuff
	fflush(stderr); 

	strExcepData+="****************************************************\n";
	strExcepData+="*** Stack Trace\n";
	strExcepData+= "****************************************************\n";

#ifndef _M_X64
	LogStackFrames(strExcepData, CodeAddress, (char *)ExInfo->ContextRecord->Ebp);
#endif

	utility::CrashRecoveryInst.MiniDumpWrite();
	sprintf(strWorkspace, "Metaglue Crash Report: %s", utility::CrashRecoveryInst.GetDNSHostname());
	utility::SendEmail( strWorkspace, strExcepData.c_str(), utility::CrashRecoveryInst.GetTmpFile() );

	printf("\n\n%s has crashed. \nA small crash report may have been sent to Metaglue Support, solely for debugging purposes\n\n", PathToExecutable);
	fflush(stdout);
	fflush(stderr); 
	exit(0);
	return EXCEPTION_EXECUTE_HANDLER;
}

namespace utility {

CrashRecovery::CrashRecovery(void)
{
	WSADATA wsaData;
	WORD wVersionRequested = MAKEWORD( 2, 2 );

	/*int err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) {
		// * We could not find a usable WinSock DLL.

	}
	gethostname(hostname,256);
	*/
	LPTOP_LEVEL_EXCEPTION_FILTER NextFilter=SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER) ::Win32FaultHandler);

	ProcessId=GetCurrentProcessId();
	hProcess=GetCurrentProcess();

	//now get some data while we are stable
	DWORD dwRet = ::GetModuleFileName(NULL, PathToExecutable, _MAX_PATH);

	DWORD szhostname= sizeof dnshostname;
	BOOL reult= GetComputerNameEx(ComputerNameDnsFullyQualified,dnshostname, &szhostname);


}

CrashRecovery::~CrashRecovery(void)
{
}

void CrashRecovery::MiniDumpWrite()
{
	char tmppath[UTIL_MAX_PATH];
	// Get the temp path.
	DWORD RetVal = GetTempPath(UTIL_MAX_PATH,  tmppath); // buffer for path 
	if (RetVal > UTIL_MAX_PATH || ( RetVal == 0)) return ;

	// Create a temporary file. 
	UINT uRetVal = GetTempFileName(tmppath, // directory for tmp files
		"METAGLUE_",        // temp file name prefix 
		0,            // create unique name 
		tmpfile);  // buffer for name 
	if (uRetVal == 0) return;

	HANDLE hDump_File = CreateFile(tmpfile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);

	MiniDumpWriteDump( (HANDLE) hProcess, ProcessId,hDump_File,MiniDumpNormal, NULL, NULL, NULL );

	CloseHandle(hDump_File);
}


} //namespace utility

#endif  //_WIN32
