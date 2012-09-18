# Makefile for libwrapmxf

include ../build/make/standard.mk


# Output binary
TARGETDIR=$(DESTDIR)/lib
TARGET := $(TARGETDIR)/libprocesswrap.a

OBJS := \
	$(OBJSDIR)/process.o \
	$(OBJSDIR)/process_metadata.o \
	$(OBJSDIR)/process_utils.o \
	$(OBJSDIR)/utf8.o \


INCLUDE+= -I../mxfwrap 
INCLUDE += -I$(MXFBASE)/include

CXXFLAGS+= -fPIC 
#CXXFLAGS += -DUNAME=\"$(shell uname -m)\"

all:$(TARGET)

$(TARGET): $(PREREQDIR)/marker $(OBJSDIR)/marker $(TARGETDIR)/marker  $(OBJS) Makefile
	$(AR) $(ARFLAGS) $(TARGET) $(OBJS) 
	$(RANLIB) $(RANLIBFLAGS) $(TARGET)

include ../build/make/rules.mk

