
include ../build/make/standard.mk


INCLUDE+= 
OBJSDIR=obj
BINDIR=$(DESTDIR)/bin
LIBDIR=$(DESTDIR)/lib


# Output binary
TARGETDIR := $(BINDIR)
TARGET := $(BINDIR)/mxfcrypt

INCLUDE += -I../include

# List of our object files
OBJS := \
	$(OBJSDIR)/crypto_asdcp.o \
	$(OBJSDIR)/mxfcrypt.o \

$(TARGET):  $(OBJSDIR)/marker $(BINDIR)/marker $(TARGETDIR)  $(OBJS) $(DESTDIR)/lib/libmxf.a
	$(CC) -o $(TARGET) $(OBJS) $(LIBRARIES)


include ../build/make/rules.mk
