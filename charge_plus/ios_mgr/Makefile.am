AM_CFLAGS = $(GLOBAL_CFLAGS) -I$(top_srcdir) -I./src `pkg-config --cflags glib-2.0`

AM_CXXFLAGS = -Wall -g -Wno-write-strings -fpermissive  -std=c++11

AUTOMAKE_OPTIONS = subdir-objects
noinst_PROGRAMS=ios_control
ios_control_SOURCES = \
		       src/idevice_ctrl.cpp \
		       src/ios_mgr.cpp \
		       src/ios_shm_mgr.cpp \
		       src/main.cpp
ios_control_LDADD=$(LIBOBJS) $(ALLOCA) $(top_srcdir)/public_func/libpublic.la $(top_srcdir)/bin/libjson.so
ios_control_CFLAGS=$(AM_CFLAGS)
ios_control_LDFLAGS=-D_GNU_SOURCE
DEFS+=-D_GNU_SOURCE
LIBS=-lpthread  -ldl -lcurl -lsqlite3 -lglib-2.0 -lgthread-2.0 -lssl -lusb-1.0 -lcrypto