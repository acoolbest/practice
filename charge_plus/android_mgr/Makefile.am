AM_CFLAGS = $(GLOBAL_CFLAGS) -I$(top_srcdir) -I./src `pkg-config --cflags glib-2.0`

AM_CXXFLAGS = -Wall -g -Wno-write-strings -fpermissive  -std=c++11

AUTOMAKE_OPTIONS = subdir-objects
noinst_PROGRAMS=android_control
android_control_SOURCES = \
		       src/adb_ctrl.cpp \
		       src/android_mgr.cpp \
		       src/android_shm_mgr.cpp \
		       src/main.cpp
android_control_LDADD=$(LIBOBJS) $(ALLOCA) $(top_srcdir)/public_func/libpublic.la $(top_srcdir)/bin/libjson.so 
android_control_CFLAGS=$(AM_CFLAGS)
android_control_LDFLAGS=-D_GNU_SOURCE
DEFS+=-D_GNU_SOURCE
LIBS=-lpthread  -ldl -lcurl -lsqlite3 -lglib-2.0 -lgthread-2.0 -lssl -lusb-1.0 -lcrypto

