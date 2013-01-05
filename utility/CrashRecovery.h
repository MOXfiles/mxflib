#ifndef _CRASH_RECOVERY_H_
#define _CRASH_RECOVERY_H_

namespace utility {
#ifdef _WIN32

#define UTIL_MAX_PATH 260

bool  SendEmail( const char * subject, const char * message, const char * DumpFileName=0); //send an email to metaglue support


class CrashRecovery
{
public:
	CrashRecovery(void);
	~CrashRecovery(void);

	void MiniDumpWrite();

	const char * GetTmpFile() const { return tmpfile; }

	const char * GetHostname() const { return hostname; };
	const char * GetDNSHostname() const { return dnshostname; };
private:

	void * hProcess;
	unsigned long  ProcessId;
	char tmpfile[UTIL_MAX_PATH];
	char hostname[256];
	char dnshostname[1024];
};

#ifndef CRASH_IMPLEMENTATION

#define __CRASH_EXTERN

#ifndef BUILD_MACHINE
#define BUILD_MACHINE Unknown
#endif

#define  STRINGIFY(x) " " #x " "  //stringifies and makes sure there is a valid string so that it will build
#define TOSTRING(x) STRINGIFY(x)


 const char * __CrashImpl_BuildTime=__TIMESTAMP__;
 const char * __CrashImpl_ImplFile=TOSTRING(BUILD_INSTANTIATE);
 const char * __CrashImpl_ImplMachine=TOSTRING(BUILD_MACHINE); 
#else

#define __CRASH_EXTERN extern 

#endif
//make sure it is not instantiated in crashrecovery.cpp
__CRASH_EXTERN CrashRecovery CrashRecoveryInst;
//coded this way, with the apparent duplication 
//so that it is immediately obvious if we make an error when changing something
extern  const char * __CrashImpl_BuildTime;
extern  const char * __CrashImpl_ImplFile;
extern  const char * __CrashImpl_ImplMachine;



long Win32FaultHandler(struct _EXCEPTION_POINTERS *  ExInfo);
#endif  // _WIN32

} //namespace utility 



#endif //_CRASH_RECOVERY_H_
