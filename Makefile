## top level makefile for mxflib.

#lower level makefiles support
# make
# make clean
# make release
#
# in addition they support the following variables:

# RELEASE=1 as an alternative way of building a release version
# EXPAT=1 to use libexpat

MXFLIB_ROOT := $(shell pwd)

EXPAT := $(shell if [ -r /usr/include/expat.h ] ; then echo 1; elif [ "$(EXPAT_ROOT)" != "" ] ; then echo 1; else echo 0; fi)

UUID := $(shell if [ -r /usr/include/uuid/uuid.h ] ; then echo 1; else echo 0; fi)

include build/make/standard.mk

DIRS := utility mxflib dictconvert libprocesswrap mxfdump mxfsplit mxfwrap simplewrap

MAKEDIRS:= $(dir $(foreach dir, $(DIRS), $(wildcard $(dir)/[Mm]akefile)))

.PHONY: $(MAKEDIRS) make_all all release clean cleanrelease

all: dirs
	$(MAKE) make_all MAKEALLOPT= MXFLIB_ROOT=$(MXFLIB_ROOT) EXPAT=$(EXPAT) UUID=$(UUID)

release: dirs
	$(MAKE) make_all MAKEALLOPT="RELEASE=1" MXFLIB_ROOT=$(MXFLIB_ROOT) EXPAT=$(EXPAT) UUID=$(UUID)

clean: dirs
	$(MAKE) make_all MAKEALLOPT="clean" MXFLIB_ROOT=$(MXFLIB_ROOT) EXPAT=$(EXPAT) UUID=$(UUID)
	$(MAKE) cleanrelease MXFLIB_ROOT=$(MXFLIB_ROOT) EXPAT=$(EXPAT) UUID=$(UUID)

cleanrelease: dirs
	$(MAKE) make_all MAKEALLOPT="clean RELEASE=1" MXFLIB_ROOT=$(MXFLIB_ROOT) EXPAT=$(EXPAT) UUID=$(UUID)

make_all: $(MAKEDIRS)

$(MAKEDIRS):
	$(MAKE) -C $@ $(MAKEALLOPT) MXFLIB_ROOT=$(MXFLIB_ROOT) EXPAT=$(EXPAT) UUID=$(UUID)

mxflib: utility

dictconvert: utility mxflib

libprocesswrap: mxflib

mxfdump: utility mxflib

mxfsplit: utility mxflib

mxfwrap: utility mxflib libprocesswrap

simplewrap: utility mxflib

mxfcrypt: utility mxflib

mxf2dot: utility mxflib

.PHONY: make
make:
	build/make/makemake.sh

.PHONY: dirs
dirs:
	mkdir -p $(DESTDIR)
	mkdir -p $(BINDIR)
	touch $(DESTDIR)/marker
