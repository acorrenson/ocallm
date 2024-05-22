all: libmyopencl.so

libmyopencl.so: stubs.c
	clang -shared -framework OpenCL -I $$(ocamlc -where) -o libmyopencl.so stubs.c

clean:
	rm libmyopencl.so
