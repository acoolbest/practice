cc=g++
tag=a.out
src=*.cpp

$(tag):$(src)
	$(cc) -g -Wall $^ -o $@ -lpthread

.PHONY:clean
clean:
	rm $(tag)