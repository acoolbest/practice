AM_CFLAGS = $(GLOBAL_CFLAGS) -I$(top_srcdir) -I./src `pkg-config --cflags glib-2.0`

AM_CXXFLAGS = -Wall -g -Wno-write-strings -fpermissive  -std=c++11


AUTOMAKE_OPTIONS = subdir-objects
noinst_LTLIBRARIES = libpublic.la
libpublic_la_SOURCES = \
		       src/json_read.cpp \
		       src/public_function.cpp \
libpublic_la_LIBADD=$(LIBOBJS) $(top_srcdir)/bin/libjson.so
libpublic_la_CFLAGS=$(AM_CFLAGS)
LIBS=-lpthread  -ldl -lcurl -lsqlite3 -lglib-2.0 -lgthread-2.0 -lssl -lusb-1.0 -lcrypto -shared
