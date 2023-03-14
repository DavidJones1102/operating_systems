all:
	make lib_static
	make lib_shared
	make dllopen
lib_static:
	gcc -Wall -c wclib.c
	ar rcs lib_wc.a wclib.o
	gcc repl.c -o repl_static.exe -L. wclib.o
lib_shared:
	gcc -Wall -fPIC -c wclib.c
	gcc -Wall -shared -o libwc.so wclib.o
	gcc -L. -Wl,-rpath=. -Wall -o repl_shared.exe repl.c -lwc 
dllopen:
	gcc -Wall -fPIC -c wclib.c
	gcc -Wall -shared -o libwc.so wclib.o
	gcc -Wall repl.c -o repl_dlopen.exe -ldl -D USE_DLL
clear:
	rm -f *.o *.a *.so *.exe


