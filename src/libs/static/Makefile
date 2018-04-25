.PHONY: all clean run

all: libstatic_lib.a

run: main
	./main

main: print_add.o libstatic_lib.a
	g++ -o main print_add.o -L. -lstatic_lib -static

libstatic_lib.a: add.o
	ar rcs libstatic_lib.a $^
	ranlib libstatic_lib.a

%.o: %.cpp
	g++ -std=c++14 -c -o $@ $^

clean:
	rm -rf main libstatic_lib.a *.o