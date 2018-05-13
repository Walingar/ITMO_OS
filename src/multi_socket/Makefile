.PHONY: server client clean

server: compile_server
	./compile_server 3425

compile_server: echo_server.cpp
	g++ -std=c++14 $^ -o $@

client: compile_client
	./compile_client 3425

compile_client: echo_client.cpp
	g++ -std=c++14 $^ -o $@

clean:
	rm -rf compile_client compile_server