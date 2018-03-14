#!/bin/sh 
#===================== 
#make all  
#===================== 
export LD_LIBRARY_PATH=/lib:/usr/lib:$LD_LIBRARY_PATH

if [ "$1"x != "clean"x ]; then
	touch NEWS README ChangeLog AUTHORS
	aclocal
	autoconf
	autoheader
	libtoolize --automake --copy --debug --force
	automake --add-missing
	#./configure
	#ac_cv_func_malloc_0_nonnull=yes ./configure --with-gnu-ld
	#make
else
	rm -rf NEWS README ChangeLog AUTHORS autom4te.cache/ INSTALL install-sh missing aclocal.m4 COPYING ltmain.sh libtool depcomp compile ar-lib stamp-h1
	rm -rf config.guess config.sub configure config.h.in config.log config.h config.status 
	rm -rf $(find ./ -name Makefile.in)
	rm -rf $(find ./ -name Makefile)
	rm -rf $(find ./ -name *~)
	rm -rf $(find ./ -name *.dirstamp*)
	rm -rf $(find ./ -name *.libs*)
	rm -rf $(find ./ -name *.deps*)
fi
