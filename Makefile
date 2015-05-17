compile:
	clang -o get_servers get_servers.c

run:
	make compile
	./get_servers

clean:
	rm get_servers

test:
	make compile
	valgrind ./get_servers
