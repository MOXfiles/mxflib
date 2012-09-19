
include ../build/make/standard.mk


INCLUDE+= 
OBJSDIR=obj
BINDIR=$(DESTDIR)/bin
LIBDIR=$(DESTDIR)/lib


# Output binary
TARGETDIR := $(BINDIR)
TARGET := $(TARGETDIR)/libutility.a

INCLUDE += -I../include

# List of our object files
OBJS := \
	$(OBJSDIR)/osprintf.o \
	$(OBJSDIR)/oxstream.o \
	$(OBJSDIR)/timecode.o \
	$(OBJSDIR)/utf8.o \

$(TARGET): $(PREREQDIR)/marker $(OBJSDIR)/marker $(TARGETDIR)/marker  $(OBJS)
	$(AR) $(ARFLAGS) $(TARGET) $(OBJS)
	$(RANLIB) $(RANLIBFLAGS) $(TARGET)

include ../build/make/rules.mk
