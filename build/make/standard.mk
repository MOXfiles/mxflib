

#path to base of mxflib checkout
MXFBASE=$(MXFLIB_ROOT)


# Figure out output directory and other platform-specific stuff
PLATFORM := $(shell uname -s)
MACHINE := $(shell uname -m)

ifeq ($(PLATFORM),Linux)

	PLATDIR := $(MACHINE)Linux

	LIBEXT=.so

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

	CXXFLAGS+= -fno-common
	LIBEXT=.dylib

else ifeq ($(findstring CYGWIN,$(PLATFORM)),CYGWIN)
	PLATDIR := Windows
	CXXFLAGS+= -DUSE_STAT_NOT_STAT64
else 
	$(error Unknown platform $(PLATFORM))
endif

DESTDIR := $(MXFBASE)/build/$(PLATDIR)

ifeq ($(UUID),1)
	LIBUUID := -luuid
	LIBRARIES += $(LIBUUID)
	CXXFLAGS += HAVE_UUID_GENERATE
endif

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

AR=ar
ARFLAGS= cru
RANLIB=ranlib
RANLIBFLAGS=


# pick up environment variable INCLUDE if not empty
ifneq "$(strip $(INCLUDE))"  ""
	INCLUDE:=-I$(INCLUDE)
endif


#location of signature file 
SIG_DIR=/proj/mxflib  

OBJSDIR := obj
COBJSDIR := c_obj


CC=g++
#DEBUGFLAGS+= -Wall

ifdef RELEASE
  OBJSDIR := $(OBJSDIR)_rel
  DESTDIR := $(DESTDIR)/release
else
  DESTDIR := $(DESTDIR)/debug
endif


PREREQDIR=prereq
CPREREQDIR=c_prereq

INCLUDE+=-I$(MXFBASE)

# C++ Flags
CXXFLAGS +=  $(DEBUGFLAGS) $(OPTIMFLAGS) -Wno-deprecated
CXXFLAGS += -D_LARGEFILE_SOURCE  -D_LARGEFILE64_SOURCE -D_FILE_OFFSET_BITS=64
ifeq ($(EXPAT),1)
CXXFLAGS += -DHAVE_EXPAT
	LIBRARIES+= -lexpat
	INCLUDE += -I$(EXPAT_ROOT)/lib
endif

BINDIR=$(DESTDIR)/bin
