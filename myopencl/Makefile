all: libmyopencl.so

libmyopencl.so: stubs.c
	clang -O3 -shared -framework OpenCL -I $$(ocamlc -where) -o libmyopencl.so stubs.c

clean:
	rm libmyopencl.so
