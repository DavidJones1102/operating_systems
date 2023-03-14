all:
	make lib_static
	make lib_shared
	make dllopen
lib_static:
	gcc -Wall $(FLAG) -c wclib.c
	ar rcs lib_wc.a wclib.o
	gcc $(FLAG) repl.c -o repl_static.exe -L. wclib.o
lib_shared:
	gcc -Wall $(FLAG) -fPIC -c wclib.c
	gcc -Wall $(FLAG) -shared -o libwc.so wclib.o
	gcc -L. -Wl,-rpath=. -Wall $(FLAG) -o repl_shared.exe repl.c -lwc 
dllopen:
	gcc -Wall $(FLAG) -fPIC -c wclib.c
	gcc -Wall $(FLAG) -shared -o libwc.so wclib.o
	gcc -Wall $(FLAG) repl.c -o repl_dlopen.exe -ldl -D USE_DLL
o:
	make all FLAG="-O"

o0:
	make all FLAG="-O0"

o1:
	make all FLAG="-O1"

o2:
	make all FLAG="-O2"

o3:
	make all FLAG="-O3"

os:
	make all FLAG="-Os"
start_tests:
	make o
	make test SUFF="O"
	make o0
	make test SUFF="O0"
	make o1
	make test SUFF="O1"
	make o2
	make test SUFF="O2"
	make o3
	make test SUFF="O3"
	make os
	make test SUFF="Os"
	rm -f test.in
test:
	make create_test
	./repl_static.exe < test.in > results_static_$(SUFF).txt 2>/dev/null
	./repl_shared.exe < test.in > results_shared_$(SUFF).txt 2>/dev/null
	./repl_dlopen.exe < test.in > results_dynamic_$(SUFF).txt 2>/dev/null

create_test:
	echo "INIT 100" > test.in
	(ls -p | grep -v test.in |sed 's/^/COUNT /') >> test.in
	echo "SHOW 0" >> test.in
	echo "SHOW 1" >> test.in
	echo "SHOW 2" >> test.in
	echo "SHOW 3" >> test.in
	echo "SHOW 4" >> test.in	
	echo "SHOW 5" >> test.in
	echo "SHOW 6" >> test.in
	echo "SHOW 7" >> test.in
	
	echo "DELETE 0" >> test.in
	echo "DELETE 1" >> test.in
	echo "DELETE 2" >> test.in
	echo "DELETE 3" >> test.in
	echo "DELETE 4" >> test.in	
	echo "DELETE 5" >> test.in
	echo "DELETE 6" >> test.in
	echo "DELETE 7" >> test.in

	echo "DESTROY" >> test.in
	echo "EXIT" >> test.in
clear:
	rm -f *.o *.a *.so *.exe *.txt test.in


