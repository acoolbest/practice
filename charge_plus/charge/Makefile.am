AM_CXXFLAGS = $(GLOBAL_CFLAGS) -I$(top_srcdir) -I./src `pkg-config --cflags glib-2.0` -Wall -g -Wno-write-strings -fpermissive  -std=c++11 

AUTOMAKE_OPTIONS = subdir-objects

noinst_PROGRAMS=charge
charge_SOURCES = \
		       src/adb_ctrl.cpp \
		       src/android_mgr.cpp \
		       src/android_shm_mgr.cpp \
		       src/device_data.cpp \
		       src/heartbeat.cpp \
		       src/idevice_ctrl.cpp \
		       src/ios_mgr.cpp \
		       src/ios_shm_mgr.cpp \
		       src/listen_usb.cpp \
		       src/main.cpp \
		       src/manager.cpp \
		       src/power_control.cpp \
		       src/thread_pool.cpp \
		       src/usb_action_data.cpp
charge_LDADD=$(LIBOBJS) $(ALLOCA) $(top_srcdir)/serial_mgr/libcom.la  $(top_srcdir)/log_mgr/libSendLog.la $(top_srcdir)/public_func/libpublic.la $(top_srcdir)/bin/libjson.so
charge_CFLAGS=$(AM_CXXFLAGS)
charge_LDFLAGS=-D_GNU_SOURCE -L/usr/lib/x86_64-linux-gnu
DEFS+=-D_GNU_SOURCE
LIBS=-lusb-1.0 -lcrypto -lpthread -ldl -lcurl -lsqlite3 -lglib-2.0 -lgthread-2.0 -lssl 
