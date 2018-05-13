.PHONY: server client clean

server: main_server
	./main_server 3425

main_server: server.cpp
	g++ -std=c++14 $^ -o $@

client: main_client
	./main_client 3425

main_client: client.cpp
	g++ -std=c++14 $^ -o $@

clean:
	rm -rf main_client main_server 