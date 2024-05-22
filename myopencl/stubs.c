#include <OpenCL/opencl.h>
#include <caml/mlvalues.h>
#include <stdio.h>
#include <stdlib.h>

void run_kernel(const char *str) {
  int err;
  printf("runing kernel %s\n", str);

  cl_device_id device_id;
  cl_uint num_devices;

  // Connect to a compute device
  err = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_ALL, 1, &device_id, &num_devices);
  if (err != CL_SUCCESS) {
    printf("Error: Failed to create a device group!\n");
    exit(EXIT_FAILURE);
  }
}

void run_kernel_stub(value str) {
  const char *c_str = String_val(str);
  run_kernel(c_str);
  return;
}