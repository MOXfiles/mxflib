# $Id$
#

# mxfcrypt compilation is enabled with ./configure --enable-crypt
# OpenSSL provides the libcrypto library for AES routines.
if HAVE_OPENSSL

INCLUDES = -I$(top_builddir)

bin_PROGRAMS = mxfcrypt

mxfcrypt_SOURCES = mxfcrypt.cpp crypto_asdcp.cpp

LDADD = ../mxflib/libmxf.a $(UUIDLIB) -lcrypto

endif
