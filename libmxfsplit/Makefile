# Makefile for libmxfsplit

include ../build/make/standard.mk

TARGETDIR=$(DESTDIR)/lib

TARGET := $(TARGETDIR)/libmxfsplit.a

OBJS := \
	$(OBJSDIR)/audiosink.o \
	$(OBJSDIR)/containerinfo.o \
        $(OBJSDIR)/libmxfsplit.o \
	$(OBJSDIR)/testmxfsplit.o \

INCLUDE += -I$(MXFBASE)/include

CXXFLAGS += -fPIC 
CXXFLAGS += -DTRAITS_HACK

all : $(TARGET)

$(TARGET) : $(PREREQDIR)/marker $(OBJSDIR)/marker $(TARGETDIR)/marker  $(OBJS) Makefile
	$(AR) $(ARFLAGS) $(TARGET) $(OBJS)
	$(RANLIB) $(RANLIBFLAGS) $(TARGET)

include ../build/make/rules.mk
