
include ../build/make/standard.mk


INCLUDE+= 
OBJSDIR=obj
BINDIR=$(DESTDIR)/bin
LIBDIR=$(DESTDIR)/lib


# Output binary
TARGETDIR := $(BINDIR)
TARGET := $(BINDIR)/simplewrap

INCLUDE += -I../include

# List of our object files
OBJS := \
	$(OBJSDIR)/simplewrap.o \

$(TARGET):  $(PREREQDIR)/marker $(OBJSDIR)/marker $(BINDIR)/marker $(TARGETDIR)  $(OBJS) $(DESTDIR)/lib/libmxf.a
	$(CC) -o $(TARGET) $(OBJS) $(LIBRARIES) $(DESTDIR)/lib/libmxf.a


include ../build/make/rules.mk
