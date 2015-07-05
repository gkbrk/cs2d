compile:
	clang -o get_servers `xml2-config --cflags` -lxml2 get_servers.c libcs2d.c

run:
	make compile
	./get_servers

clean:
	rm get_servers

test:
	make compile
	valgrind ./get_servers
