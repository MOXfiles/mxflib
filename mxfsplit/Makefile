# Makefile for mxfsplit

include ../build/make/standard.mk


INCLUDE+= 
OBJSDIR=obj
BINDIR=$(DESTDIR)/bin
LIBDIR=$(DESTDIR)/lib


# Output binary
TARGETDIR := $(BINDIR)
TARGET := $(BINDIR)/mxfsplit

INCLUDE += -I../include

# List of our object files
OBJS := \
	$(OBJSDIR)/mxfsplit.o \

$(TARGET):  $(PREREQDIR)/marker $(OBJSDIR)/marker $(BINDIR)/marker $(TARGETDIR)/marker  $(OBJS) $(DESTDIR)/lib/libmxf.a $(DESTDIR)/lib/libutility.a $(DESTDIR)/lib/libmxfsplit.a
	$(CC) -o $(TARGET) $(OBJS) $(DESTDIR)/lib/libmxf.a $(DESTDIR)/lib/libutility.a $(DESTDIR)/lib/libmxfsplit.a $(LIBRARIES)


include ../build/make/rules.mk
