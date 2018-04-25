.PHONY: all clean run

all: libshared_lib.so

run: main
	./main

main: print_sub.o libshared_lib.so
	g++ -o main print_sub.o -L. -lshared_lib -Wl,-rpath,.

%.o: %.cpp
	g++ -std=c++14 -fPIC -c -o $@ $^

libshared_lib.so: sub.o
	g++ -std=c++14 -shared -o libshared_lib.so $^

clean:
	rm -rf libshared_lib.so *.o main