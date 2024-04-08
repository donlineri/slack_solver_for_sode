CFLAGS = -ggdb -Wall -lm
math: math.c
	$(CC) $(CFLAGS) $^ -o $@
math.cgi: math.c
	$(CC) $(CFLAGS) -DCGI $^ -o $@
install: math.cgi
	cp math.cgi /var/www/filetrash/cgi/
