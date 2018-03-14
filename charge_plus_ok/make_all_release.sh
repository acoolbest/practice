#!/bin/sh 
#===================== 
#make all  
#===================== 

cd ./third_party

cd ./libjson
make clean
if [ "$1"x != "clean"x ]; then
make
fi

cd ../libusbx
make distclean
if [ "$1"x != "clean"x ]; then
./configure
fi

cd ..

cd ../public_func
make clean -f makefile_lib
if [ "$1"x != "clean"x ]; then
make ver=release -f makefile_lib
fi

cd ../http_req
make clean -f makefile_lib
#if [ "$1"x != "clean"x ]; then
#make ver=release -f makefile_lib
#fi

cd ../log_mgr
make clean -f makefile_lib
make clean -f makefile_xy
if [ "$1"x != "clean"x ]; then
#make ver=release -f makefile_lib
make ver=release -f makefile_xy
fi

cd ../serial_mgr
make clean -f makefile_lib
if [ "$1"x != "clean"x ]; then
make ver=release -f makefile_lib
fi

cd ../android_mgr
make clean
if [ "$1"x != "clean"x ]; then
make ver=release
fi

cd ../ios_mgr
make clean
if [ "$1"x != "clean"x ]; then
make ver=release
fi

cd ../charge
make clean
if [ "$1"x != "clean"x ]; then
make ver=release
fi

cd ../update
./cmakeclean.sh
if [ "$1"x != "clean"x ]; then
cmake .
make
fi
