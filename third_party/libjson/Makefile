CC = g++
srcdir          ?= _internal/Source
CFLAGS = -Wall -g -Wno-write-strings -fpermissive
INCLUDE = -I. 
LDFLAGS =      -shared

TARDIR = ../../bin
LIBRARY = libjson.so
OBJECTS = internalJSONNode.o JSONAllocator.o JSONChildren.o \
		  JSONDebug.o JSONIterators.o JSONMemory.o JSONNode.o \
		  JSONNode_Mutex.o JSONPreparse.o JSONStream.o JSONValidator.o \
		  JSONWorker.o JSONWriter.o libjson.o 

$(LIBRARY): $(OBJECTS)
	@if test ! -d $(TARDIR); \
	then \
		mkdir $(TARDIR); \
	fi
	$(CC) $(CFLAGS) -o $(TARDIR)/$(LIBRARY) $(OBJECTS) $(LDFLAGS)
	#cp $(LIBRARY) ${TARDIR}

%.o: $(srcdir)/%.cpp
	$(CC) $(CFLAGS) $(INCLUDE) -fPIC -c $< -o $@

clean:
	rm -f *.o core.* $(TARDIR)/$(LIBRARY)
