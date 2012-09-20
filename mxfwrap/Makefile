# Makefile for MXFWRAP

include ../build/make/standard.mk


INCLUDE+= 
OBJSDIR=obj
BINDIR=$(DESTDIR)/bin
LIBDIR=$(DESTDIR)/lib


# Output binary
TARGETDIR := $(BINDIR)
TARGET := $(BINDIR)/mxfwrap

INCLUDE += -I../include
LIBRARIES += $(DESTDIR)/lib/libprocesswrap.a

# List of our object files
OBJS := \
	$(OBJSDIR)/mxfwrap.o \
	$(OBJSDIR)/parseoptions.o \

$(TARGET):  $(PREREQDIR)/marker $(OBJSDIR)/marker $(BINDIR)/marker $(TARGETDIR)/marker  $(OBJS) $(DESTDIR)/lib/libmxf.a
	$(CC) -o $(TARGET) $(OBJS) $(LIBRARIES) $(DESTDIR)/lib/libmxf.a


include ../build/make/rules.mk
