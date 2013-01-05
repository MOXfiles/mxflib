
#path to base of mxflib checkout
MXFBASE = $(MXFLIB_ROOT)

# Figure out output directory and other platform-specific stuff
PLATFORM := $(shell uname -s)
MACHINE := $(shell uname -m)

ifeq ($(PLATFORM),Linux)

	PLATDIR := $(MACHINE)Linux
	
	LIBEXT = .so

else ifeq ($(PLATFORM),SunOS)

	PLATDIR := SparcSolaris

else ifeq ($(PLATFORM),IRIX)

	PLATDIR := MipsIrix

else ifeq ($(PLATFORM),Darwin)

	ifeq ($(MACHINE),Power Macintosh)
		PLATDIR := PPCDarwin
	else
		PLATDIR := $(MACHINE)Darwin
	endif

	CXXFLAGS += -fno-common
	LIBEXT = .dylib

	ifeq ($(UUID),1)
		# from Lion onwards, uuid functions are in libc, not libuuid
		# the following expression does unnatural things and yields a number like 100704
		UUID = $(shell if [[ `sw_vers | grep 'Product.*Version:' | grep -o '[0-9]*\.[0-9]*\.[0-9]*' | sed 's/\.\(1[0-9]\)/\1/g' | sed 's/\./0/g'` < 100700 ]] ; then echo 1; else echo 2; fi )
	endif
	
else ifeq ($(findstring CYGWIN,$(PLATFORM)),CYGWIN)

	PLATDIR := Windows
	CXXFLAGS+= -DUSE_STAT_NOT_STAT64
	
else 

	$(error Unknown platform $(PLATFORM))
	
endif

DESTDIR := $(MXFBASE)/build/$(PLATDIR)

ifeq ($(MACHINE),x86_64)
	SIXTYFOURBIT:= "64"
endif

ifdef RELEASE
	DEBUGFLAGS :=
	OPTIMFLAGS := -O2
else
  DEBUGFLAGS := -DDEBUG -g
  OPTIMFLAGS :=
endif

AR = ar
ARFLAGS = cru
RANLIB = ranlib
RANLIBFLAGS =

# pick up environment variable INCLUDE if not empty
ifneq "$(strip $(INCLUDE))"  ""
	INCLUDE := -I$(INCLUDE)
endif

INCLUDE += -I$(MXFBASE)

OBJSDIR := obj
COBJSDIR := c_obj

CC=g++

ifdef RELEASE
	OBJSDIR := $(OBJSDIR)_rel
	DESTDIR := $(DESTDIR)/release
else
	DESTDIR := $(DESTDIR)/debug
endif

PREREQDIR = prereq
CPREREQDIR = c_prereq

BINDIR = $(DESTDIR)/bin

# C++ Flags
CXXFLAGS += -fPIC
CXXFLAGS +=  $(DEBUGFLAGS) $(OPTIMFLAGS) -Wno-deprecated
CXXFLAGS += -D_LARGEFILE_SOURCE  -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64

ifeq ($(UUID),1)
	LIBUUID := -luuid
	LIBRARIES += $(LIBUUID)
	CXXFLAGS += -DHAVE_UUID_GENERATE
else ifeq ($(UUID),2)
	# no libuuid for Lion
	CXXFLAGS += -DHAVE_UUID_GENERATE
endif

ifdef EXPAT
	ifeq ($(EXPAT),1)
		CXXFLAGS += -DHAVE_EXPAT
		LIBRARIES += -lexpat
	
		ifeq ($(EXPAT_ROOT),)
			INCLUDE += -I/usr/include
		else
			INCLUDE += -I$(EXPAT_ROOT)/lib
		endif
	endif
endif

