.PHONY: run all

run: out
	./out

all: out

out: main.cpp
	g++ -std=c++14 $^ -o $@

clean:
	rm -rf out