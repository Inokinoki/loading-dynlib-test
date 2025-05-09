all: main.out main.local.out dyn_lib1.so dyn_lib2.so

FLAGS=-fPIC -fvisibility=hidden

static_lib_v1.a: static_lib_v1.cc
	g++ -c $(FLAGS) static_lib_v1.cc -o static_lib_v1.o
	ar rcs static_lib_v1.a static_lib_v1.o

static_lib_v2.a: static_lib_v2.cc
	g++ -c $(FLAGS) static_lib_v2.cc -o static_lib_v2.o
	ar rcs static_lib_v2.a static_lib_v2.o

dyn_lib1.so: dyn_lib1.cc static_lib_v1.a
	g++ -fPIC -shared dyn_lib1.cc static_lib_v1.a -o dyn_lib1.so

dyn_lib2.so: dyn_lib2.cc static_lib_v2.a
	g++ -fPIC -shared dyn_lib2.cc static_lib_v2.a -o dyn_lib2.so

main.out: main.cc
	g++ main.cc -o main.out

# Add RTLD_LOCAL flag to explicitly load the shared library locally
main.local.out: main.cc
	g++ -DLOAD_LOCALLY main.cc -o main.local.out

clean:
	rm -f *.so *.out *.a *.o

.PHONY: clean
