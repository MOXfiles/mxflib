# Makefile for mxfdump

include ../build/make/standard.mk


INCLUDE+= 
OBJSDIR=obj
BINDIR=$(DESTDIR)/bin
LIBDIR=$(DESTDIR)/lib


# Output binary
TARGETDIR := $(BINDIR)
TARGET := $(TARGETDIR)/mxfdump

INCLUDE += -I../include

# List of our object files
OBJS := \
	$(OBJSDIR)/mxfdump.o \


$(TARGET):  $(PREREQDIR)/marker $(OBJSDIR)/marker $(BINDIR)/marker $(TARGETDIR)/marker $(OBJS) $(DESTDIR)/lib/libmxf.a
	$(CC) -o $(TARGET) $(OBJS) $(LIBRARIES) $(DESTDIR)/lib/libmxf.a $(DESTDIR)/lib/libutility.a


include ../build/make/rules.mk
