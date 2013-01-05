# Makefile for utility

include ../build/make/standard.mk

# Output binary
TARGETDIR=$(DESTDIR)/lib
TARGET := $(TARGETDIR)/libutility.a

OBJS := \
	$(OBJSDIR)/CrashRecovery.o \
	$(OBJSDIR)/osprintf.o \
	$(OBJSDIR)/oxstream.o \
	$(OBJSDIR)/timecode.o	\
	$(OBJSDIR)/utf8.o \

INCLUDE += -I$(MXFBASE)/include

CXXFLAGS+= -DTRAITS_HACK -fPIC 

all:$(TARGET)

$(TARGET): $(PREREQDIR)/marker $(OBJSDIR)/marker $(TARGETDIR)/marker  $(OBJS)
	$(AR) $(ARFLAGS) $(TARGET) $(OBJS)
	$(RANLIB) $(RANLIBFLAGS) $(TARGET)

include ../build/make/rules.mk

