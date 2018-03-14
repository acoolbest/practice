src += main.c
src += arp_pthread.c
src += arp_link.c
src += ip_pthread.c
src += ip_link.c
src += key_pthread.c
src += get_interface.c
src += thread_pool.c
src += stftp.c
obj=$(src:%.c=%.o)
target=demo
cflags=-lpthread -Wall -g -lgthread-2.0 `pkg-config --libs --cflags gtk+-2.0`

$(target):$(obj)
	$(CC) $^ -o $@ $(cflags)
%.o:%.c
	$(CC) -c $< -o $@ $(cflags)
clean:
	rm *.o $(target)

