src += attack.c
src += arp.c


obj=$(src:%.c=%.o)
target=demo
cflags=-lpthread -Wall -lpcap -lnet -g -lgthread-2.0 `pkg-config --libs --cflags gtk+-2.0`

$(target):$(obj)
	$(CC) $^ -o $@ $(cflags)
%.o:%.c
	$(CC) -c $< -o $@ $(cflags)
clean:
	rm *.o $(target)

