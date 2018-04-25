.PHONY: all clean run

all: libloadable_lib.so

run: main
	./main

main: print_mul.o libloadable_lib.so
	g++ -std=c++14 -o main -L. print_mul.o -ldl

%.o: %.cpp
	g++ -std=c++14 -fPIC -c -o $@ $^

libloadable_lib.so: mul.o
	g++ -std=c++14 -shared -o libloadable_lib.so $^

clean:
	rm -rf libloadable_lib.so *.o main