
include ../build/make/standard.mk


INCLUDE+= 
OBJSDIR=obj
BINDIR=$(DESTDIR)/bin
LIBDIR=$(DESTDIR)/lib


# Output binary
TARGETDIR := $(BINDIR)
TARGET := $(BINDIR)/mxf2dot

INCLUDE += -I../include

# List of our object files
OBJS := \
	$(OBJSDIR)/DotFile.o \
	$(OBJSDIR)/mxf2dot.o \

$(TARGET):  $(OBJSDIR)/marker $(BINDIR)/marker $(TARGETDIR)  $(OBJS) $(DESTDIR)/lib/libmxf.a
	$(CC) -o $(TARGET) $(OBJS) $(LIBRARIES) $(DESTDIR)/lib/libmxf.a


include ../build/make/rules.mk
