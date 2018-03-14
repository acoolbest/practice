#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT(bin/charge, 1.0, acoolbest@163.com)
AC_CONFIG_SRCDIR([charge/src/main.cpp])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE([foreign -Wall]) 

# Checks for programs.
AC_PROG_AWK
AC_PROG_CXX
AC_PROG_CC
AM_PROG_AR
AC_PROG_INSTALL
AC_PROG_MAKE_SET


# Checks for libraries. lpthread  -ldl -lcurl -lsqlite3 -lglib-2.0 -lgthread-2.0 -lssl -lusb-1.0 -lcrypto -lcom -lSendLog -lpublic -ljson 
AC_CHECK_LIB([curl], [main])
AC_CHECK_LIB([sqlite3], [main])
AC_CHECK_LIB([glib-2.0], [main])
AC_CHECK_LIB([gthread-2.0], [main])
AC_CHECK_LIB([ssl], [main])
AC_CHECK_LIB([usb-1.0], [main])
AC_CHECK_LIB([com], [main])
AC_CHECK_LIB([SendLog], [main])
AC_CHECK_LIB([public], [main])
AC_CHECK_LIB([json], [main])

# FIXME: Replace `main' with a function in `-lc':
#AC_CHECK_LIB([c], [main])
# FIXME: Replace `main' with a function in `-lcrypto':
#AC_CHECK_LIB([crypto], [main])
# FIXME: Replace `main' with a function in `-lpthread':
#AC_CHECK_LIB([pthread], [main])
# FIXME: Replace `main' with a function in `-lz':
#AC_CHECK_LIB([z], [main])

# Checks for header files.
AC_HEADER_RESOLV
AC_CHECK_HEADERS([arpa/inet.h fcntl.h inttypes.h limits.h memory.h netdb.h netinet/in.h stddef.h stdint.h stdlib.h string.h strings.h sys/ioctl.h sys/mount.h sys/socket.h sys/time.h termios.h unistd.h utime.h wchar.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_UID_T
AC_C_INLINE
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_INT8_T
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_PID_T
AC_TYPE_SIZE_T
AC_TYPE_SSIZE_T
AC_CHECK_MEMBERS([struct stat.st_rdev])
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T
AC_CHECK_TYPES([ptrdiff_t])

# Checks for library functions
#AC_FUNC_ALLOCA
AC_FUNC_CHOWN
AC_FUNC_FORK
AC_FUNC_FSEEKO
#AC_FUNC_LSTAT_FOLLOWS_SLASHED_SYMLINK
AC_HEADER_MAJOR
#AC_FUNC_MALLOC
#AC_FUNC_REALLOC
AC_CHECK_FUNCS([atexit bzero dup2 ftruncate getcwd gethostbyname gethostname gettimeofday localtime_r memmove memset mkdir pow putenv select setenv socket strcasecmp strchr strdup strerror strncasecmp strndup strpbrk strrchr strstr strtol strtoul tzset utime])
AC_PROG_LIBTOOL
AC_CONFIG_FILES([Makefile
                 android_mgr/Makefile
                 charge/Makefile
                 ios_mgr/Makefile
                 log_mgr/Makefile
                 public_func/Makefile
                 serial_mgr/Makefile])
#AC_CONFIG_SUBDIRS([third_party/libusbx])
AC_OUTPUT
