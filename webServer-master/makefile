CC = g++
CFLAGS = -O2 -Wall -I .

LIB = -pthread

all: webserver

webserver: webServer.cpp locker.h thread_pool.h task.h
	$(CC) $(CFLAGS) -o webserver webServer.cpp locker.h thread_pool.h task.h $(LIB)

clean:
	rm -f *.o *~ webserver
