#!/bin/sh 
#===================== 
#make all  
#===================== 
cd ./public_func
make clean -f makefile_lib
make -f makefile_lib

cd ../http_req
make clean -f makefile_lib
#make -f makefile_lib
make clean
make

cd ../log_mgr
make clean -f makefile_lib
#make -f makefile_lib
make clean
make
make clean -f makefile_xy
make -f makefile_xy

cd ../serial_mgr
make clean -f makefile_lib
make -f makefile_lib
make clean
make
