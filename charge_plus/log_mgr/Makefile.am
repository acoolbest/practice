AM_CFLAGS = $(GLOBAL_CFLAGS) -I$(top_srcdir) -I./src `pkg-config --cflags glib-2.0`

AM_CXXFLAGS = -Wall -g -Wno-write-strings -fpermissive  -std=c++11

AUTOMAKE_OPTIONS = subdir-objects
noinst_LTLIBRARIES = libSendLog.la
libSendLog_la_SOURCES = \
		       src/libsendlog.cpp \
		       src/log_mgr.cpp \
		       src/logdata.cpp \
		       src/database.cpp \
			   ../http_req/src/http_req.cpp \
		       ../http_req/src/http_req_interface.cpp \
			   ../public_func/src/json_read.cpp \
		       ../public_func/src/public_function.cpp
libSendLog_la_LIBADD=$(LIBOBJS) $(ALLOCA) $(top_srcdir)/bin/libjson.so
libSendLog_la_CFLAGS=$(AM_CFLAGS)
LIBS=-lpthread  -ldl -lcurl -lsqlite3 -lglib-2.0 -lgthread-2.0 -lssl -lusb-1.0 -lcrypto -shared
