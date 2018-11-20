g++ -m64 -I../base -I../hiredis/hiredis-master -c -o CachePool.o CachePool.cpp
g++ -m64 -I../base -I/usr/include/mysql -c -o DBPool.o DBPool.cpp
g++ -m64 -c -o Thread.o ../base/Thread.cpp 
ar rcs libpool.a Thread.o CachePool.o DBPool.o
cp libpool.a ../lib
rm -f *.o
