#include <OpenCL/OpenCL.h>
#include <OpenCL/opencl.h>
#include <caml/bigarray.h>
#include <caml/mlvalues.h>
#include <stdio.h>
#include <stdlib.h>

struct device_s {
  cl_device_id id;           // compute device id
  cl_context context;        // compute context
  cl_uint num_devices;       // number of devices
  cl_command_queue commands; // compute command queue
  cl_program program;        // compute program
  cl_kernel kernel;          // compute kernel
};
typedef struct device_s device;

void error(char *msg) {
  fprintf(stderr, "%s\n", msg);
  exit(EXIT_FAILURE);
}

void initialize(device *dev) {
  int err;
  err =
      clGetDeviceIDs(NULL, CL_DEVICE_TYPE_ALL, 1, &dev->id, &dev->num_devices);
  if (err != CL_SUCCESS) {
    error("Error: Failed to create a device group!");
  }

  // Create a compute context
  dev->context =
      clCreateContext(NULL, dev->num_devices, &dev->id, NULL, NULL, &err);
  if (!dev->context) {
    error("Error: Failed to create a compute context!");
  }

  // Create a command commands
  dev->commands = clCreateCommandQueue(dev->context, dev->id, 0, &err);
  if (!dev->commands) {
    error("Error: Failed to create a command queue!");
  }
}

void load_kernel(device *dev, const char *name, const char *path) {
  char *kernel_source;
  int length;
  FILE *f = fopen(path, "rb");

  if (!f) {
    error("Error: Failed to load kernel!");
  }

  fseek(f, 0, SEEK_END);
  length = ftell(f);
  fseek(f, 0, SEEK_SET);
  kernel_source = malloc(length);

  fread(kernel_source, 1, length, f);
  fclose(f);

  int err;

  // Create the compute program from the source buffer
  dev->program = clCreateProgramWithSource(
      dev->context, 1, (const char **)&kernel_source, NULL, &err);
  if (!dev->program || err != CL_SUCCESS) {
    error("Error: Failed to create compute program!");
  }

  // Build the program executable
  err = clBuildProgram(dev->program, 0, NULL, NULL, NULL, NULL);
  if (err != CL_SUCCESS) {
    size_t len;
    char buffer[2048];
    clGetProgramBuildInfo(dev->program, dev->id, CL_PROGRAM_BUILD_LOG,
                          sizeof(buffer), buffer, &len);
    printf("%s\n", buffer);
    error("Error: Failed to build program executable!");
  }

  // Create the compute kernel in the program we wish to run
  dev->kernel = clCreateKernel(dev->program, name, &err);
  if (!dev->kernel || err != CL_SUCCESS) {
    error("Error: Failed to create compute kernel!");
  }
}

void vec_bop(const char *name, const char *path, float *a, float *b, float *c,
             int dim) {
  device dev;
  initialize(&dev);
  load_kernel(&dev, name, path);

  cl_int err, err_a, err_b;
  size_t size = sizeof(float) * dim;
  cl_mem memory_A =
      clCreateBuffer(dev.context, CL_MEM_READ_ONLY, size, NULL, NULL);
  cl_mem memory_B =
      clCreateBuffer(dev.context, CL_MEM_READ_ONLY, size, NULL, NULL);
  cl_mem memory_C =
      clCreateBuffer(dev.context, CL_MEM_WRITE_ONLY, size, NULL, NULL);
  if (!memory_A || !memory_B || !memory_C) {
    error("Error: Failed to allocate device memory!");
  }
  err_a = clEnqueueWriteBuffer(dev.commands, memory_A, CL_TRUE, 0, size, a, 0,
                               NULL, NULL);
  err_b = clEnqueueWriteBuffer(dev.commands, memory_B, CL_TRUE, 0, size, b, 0,
                               NULL, NULL);

  if (err_a != CL_SUCCESS || err_b != CL_SUCCESS) {
    error("Error: Failed to write to source arrays!");
  }

  err = clSetKernelArg(dev.kernel, 0, sizeof(cl_mem), &memory_C);
  err |= clSetKernelArg(dev.kernel, 1, sizeof(cl_mem), &memory_A);
  err |= clSetKernelArg(dev.kernel, 2, sizeof(cl_mem), &memory_B);

  if (err != CL_SUCCESS) {
    error("Error: Failed to set kernel arguments!");
  }

  // Get the maximum work group size for executing the kernel on the device
  size_t local;
  size_t global;
  err = clGetKernelWorkGroupInfo(dev.kernel, dev.id, CL_KERNEL_WORK_GROUP_SIZE,
                                 sizeof(local), &local, NULL);
  if (err != CL_SUCCESS) {
    printf("Error: Failed to retrieve kernel work group info! %d\n", err);
    exit(1);
  }

  // Execute the kernel over the entire range of our 1d input data set
  // using the maximum number of work group items for this device
  global = dim;
  local = dim < local ? dim : local;
  err = clEnqueueNDRangeKernel(dev.commands, dev.kernel, 1, NULL, &global,
                               &local, 0, NULL, NULL);
  if (err) {
    error("Error: Failed to execute kernel!");
  }

  // Wait for the command commands to get serviced before reading back results
  clFinish(dev.commands);

  // Read back the results from the device to verify the output
  err = clEnqueueReadBuffer(dev.commands, memory_C, CL_TRUE, 0, size, c, 0,
                            NULL, NULL);
  if (err != CL_SUCCESS) {
    error("Error: Failed to copy output array!");
  }

  clReleaseMemObject(memory_A);
  clReleaseMemObject(memory_B);
  clReleaseMemObject(memory_C);
  clReleaseProgram(dev.program);
  clReleaseKernel(dev.kernel);
  clReleaseCommandQueue(dev.commands);
  clReleaseContext(dev.context);
}

void vec_add(float *a, float *b, float *c, int dim) {
  vec_bop("vec_add", "./myopencl/kernels/vec_add.cl", a, b, c, dim);
}

void vec_mul(float *a, float *b, float *c, int dim) {
  vec_bop("vec_mul", "./myopencl/kernels/vec_mul.cl", a, b, c, dim);
}

void vec_add_stub(value a, value b, value c) {
  int dim = Caml_ba_array_val(a)->dim[0];
  float *arr_a = Caml_ba_data_val(a);
  float *arr_b = Caml_ba_data_val(b);
  float *arr_c = Caml_ba_data_val(c);
  vec_add(arr_a, arr_b, arr_c, dim);
}

void vec_mul_stub(value a, value b, value c) {
  int dim = Caml_ba_array_val(a)->dim[0];
  float *arr_a = Caml_ba_data_val(a);
  float *arr_b = Caml_ba_data_val(b);
  float *arr_c = Caml_ba_data_val(c);
  vec_mul(arr_a, arr_b, arr_c, dim);
}