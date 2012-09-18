# Makefile for MXFLIB

include ../build/make/standard.mk

OBJS := \
	$(OBJSDIR)/audiomux.o \
	$(OBJSDIR)/crypto.o \
	$(OBJSDIR)/datachunk.o \
	$(OBJSDIR)/deftypes.o \
	$(OBJSDIR)/esp.o \
	$(OBJSDIR)/esp_dvdif.o \
	$(OBJSDIR)/esp_jp2k.o \
	$(OBJSDIR)/esp_mpeg2ves.o \
	$(OBJSDIR)/esp_wavepcm.o \
	$(OBJSDIR)/essence.o \
	$(OBJSDIR)/helper.o \
	$(OBJSDIR)/index.o \
	$(OBJSDIR)/klvobject.o \
	$(OBJSDIR)/legacytypes.o \
	$(OBJSDIR)/mdobject.o \
	$(OBJSDIR)/mdtraits.o \
	$(OBJSDIR)/mdtype.o \
	$(OBJSDIR)/metadata.o \
	$(OBJSDIR)/metadict.o \
	$(OBJSDIR)/mxffile.o \
	$(OBJSDIR)/partition.o \
	$(OBJSDIR)/primer.o \
	$(OBJSDIR)/rip.o \
	$(OBJSDIR)/rxiparser.o \
	$(OBJSDIR)/sopsax.o \
	$(OBJSDIR)/types.o \
	$(OBJSDIR)/uuid.o \
	$(OBJSDIR)/vbi.o \
	$(OBJSDIR)/xmlparser.o \

# Output binary
TARGETDIR=$(DESTDIR)/lib
TARGET := $(TARGETDIR)/libmxf.a

ifeq ($(PLATFORM),Darwin)
	LIB_OPTS= -fno-common
endif


CXXFLAGS+=-fPIC


all:$(TARGET)


$(TARGET): $(PREREQDIR)/marker $(OBJSDIR)/marker $(TARGETDIR)/marker  $(OBJS)
	$(AR) $(ARFLAGS) $(TARGET) $(OBJS)
	$(RANLIB) $(RANLIBFLAGS) $(TARGET)

include ../build/make/rules.mk

