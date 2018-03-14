AM_CFLAGS = $(GLOBAL_CFLAGS) -I$(top_srcdir) -I./src `pkg-config --cflags glib-2.0`

AM_CXXFLAGS = -Wall -g -Wno-write-strings -fpermissive  -std=c++11

AUTOMAKE_OPTIONS = subdir-objects
noinst_LTLIBRARIES = libcom.la
libcom_la_SOURCES = \
		       src/com_transmission.cpp \
		       src/comm_helper.cpp \
			   src/enum_board.cpp \
			   src/inifile.cpp
libcom_la_LIBADD=$(LIBOBJS) $(ALLOCA) $(top_srcdir)/public_func/libpublic.la
libcom_la_CFLAGS=$(AM_CFLAGS)
LIBS=-lpthread  -ldl -lcurl -lsqlite3 -lglib-2.0 -lgthread-2.0 -lssl -lusb-1.0 -lcrypto -shared