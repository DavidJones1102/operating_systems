all:
	make lib_static
	make lib_shared
lib_static:
	gcc -Wall -c wclib.c
	ar rcs lib_wc.a wclib.o
lib_shared:
	gcc -Wall -fPIC -c wclib.c
	gcc -Wall -shared -o lib_wc.so wclib.o
clear:
	rm -f *.o *.a *.so


