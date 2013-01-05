# Makefile for mxfwrap

include ../build/make/standard.mk


# Output binary
TARGETDIR := $(BINDIR)
TARGET := $(TARGETDIR)/mxfwrap


# List of our object files, used to create mxfwrap executable
OBJS := \
	$(OBJSDIR)/mxfwrap.o \
	$(OBJSDIR)/parseoptions.o \

$(TARGET): $(PREREQDIR)/marker $(OBJSDIR)/marker $(TARGETDIR)/marker  $(OBJS) $(LICENCELIB) $(DESTDIR)/lib/libutility.a $(DESTDIR)/lib/libmxf.a $(DESTDIR)/lib/libprocesswrap.a $(LIBDNX)
	$(CC) $(LINKOPTS) -o $(TARGET) $(OBJS) $(LICENCELIB) $(DESTDIR)/lib/libprocesswrap.a $(DESTDIR)/lib/libmxf.a $(DESTDIR)/lib/libutility.a $(LIBDNX) $(LIBRARIES)
	$(PREPARE_LIC) 



include ../build/make/rules.mk
