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


# the main targets
.PHONY: all
all: dirs
	$(MAKE) make_all MAKEALLOPT= MXFLIB_ROOT=$(MXFLIB_ROOT) EXPAT=$(EXPAT) UUID=$(UUID)

.PHONY: release
release: dirs
	$(MAKE) make_all MAKEALLOPT="RELEASE=1" MXFLIB_ROOT=$(MXFLIB_ROOT) EXPAT=$(EXPAT) UUID=$(UUID)

.PHONY: clean
clean: dirs
	$(MAKE) make_all MAKEALLOPT="clean" MXFLIB_ROOT=$(MXFLIB_ROOT) EXPAT=$(EXPAT) UUID=$(UUID)
	$(MAKE) cleanrelease MXFLIB_ROOT=$(MXFLIB_ROOT) EXPAT=$(EXPAT) UUID=$(UUID)

.PHONY: cleanrelease
cleanrelease: dirs
	$(MAKE) make_all MAKEALLOPT="clean RELEASE=1" MXFLIB_ROOT=$(MXFLIB_ROOT) EXPAT=$(EXPAT) UUID=$(UUID)


# declare the subdirectories (only those with makefiles) as phony targets to cause recursion
DIRS := utility mxflib libprocesswrap libmxfsplit mxfsplit mxfdump mxfwrap simplewrap dictconvert

MAKEDIRS:= $(dir $(foreach dir, $(DIRS), $(wildcard $(dir)/[Mm]akefile)))

.PHONY: $(MAKEDIRS) make_all

make_all: $(MAKEDIRS)

$(MAKEDIRS):
	$(MAKE) -C $@ $(MAKEALLOPT) MXFLIB_ROOT=$(MXFLIB_ROOT) EXPAT=$(EXPAT) UUID=$(UUID)



# declare the dependencies between directories

mxflib: utility

dictconvert: utility mxflib

libprocesswrap: mxflib

mxfdump: utility mxflib

libmxfsplit: mxflib

mxfsplit: utility libmxfsplit mxflib

mxfwrap: utility mxflib libprocesswrap

simplewrap: utility mxflib

mxfcrypt: utility mxflib

mxf2dot: utility mxflib

# miscellaneous targets
.PHONY: dirs
dirs:
	mkdir -p $(DESTDIR)
	mkdir -p $(BINDIR)
	touch $(DESTDIR)/marker

.PHONY: docs
docs:
	$(MAKE) -C $@

.PHONY: tests
tests:
	$(MAKE) -C $@ MXFLIB_ROOT=$(MXFLIB_ROOT) 

.PHONY: make
make:
	build/make/makemake.sh

