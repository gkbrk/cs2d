compile:
	clang -o libcs2d.o -c libcs2d.c
	clang -o get_servers get_servers.c libcs2d.o

run:
	make compile
	./get_servers

clean:
	rm get_servers
	rm libcs2d.o
	rm libcs2d.a

test:
	make compile
	valgrind ./get_servers
