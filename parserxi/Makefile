# Makefile for parserxi

include ../build/make/standard.mk


INCLUDE+= 
OBJSDIR=obj
BINDIR=$(DESTDIR)/bin
LIBDIR=$(DESTDIR)/lib


# Output binary
TARGETDIR := $(BINDIR)
TARGET := $(BINDIR)/parserxi

INCLUDE += -I../include

# List of our object files
OBJS := \
	$(OBJSDIR)/parserxi.o \


$(TARGET):  $(PREREQDIR)/marker $(OBJSDIR)/marker $(BINDIR)/marker $(TARGETDIR)/marker $(OBJS) $(DESTDIR)/lib/libmxf.a
	$(CC) -o $(TARGET) $(OBJS) $(DESTDIR)/lib/libmxf.a $(LIBRARIES)


include ../build/make/rules.mk
