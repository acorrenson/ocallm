#include <OpenCL/opencl.h>
#include <stdio.h>
#include <stdlib.h>

#define COUNT 1024

int main(int argc, char const *argv[]) {

  // API calls errors
  int err;

  unsigned int count = COUNT;

  // Data
  int A[COUNT];
  int B[COUNT];
  int C[COUNT];

  // OpenCL stuff

  size_t global; // global domain size for our calculation
  size_t local;  // local domain size for our calculation

  cl_device_id device_id;    // compute device id
  cl_context context;        // compute context
  cl_command_queue commands; // compute command queue
  cl_program program;        // compute program
  cl_kernel kernel;          // compute kernel

  cl_mem memory_inp_A; // device memory used for the input array
  cl_mem memory_inp_B; // device memory used for the input array
  cl_mem memory_out;   // device memory used for the output array

  // Fill our data set with random integer values
  int i = 0;
  for (i = 0; i < COUNT; i++) {
    A[i] = i;
    B[i] = i;
  }

  cl_uint num_devices;

  // Connect to a compute device
  err = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_ALL, 1, &device_id, &num_devices);
  if (err != CL_SUCCESS) {
    printf("Error: Failed to create a device group!\n");
    return EXIT_FAILURE;
  }

  printf("got %d device(s)\n", num_devices);

  // Create a compute context
  context = clCreateContext(0, 1, &device_id, NULL, NULL, &err);
  if (!context) {
    printf("Error: Failed to create a compute context!\n");
    return EXIT_FAILURE;
  }

  // Create a command commands
  commands = clCreateCommandQueue(context, device_id, 0, &err);
  if (!commands) {
    printf("Error: Failed to create a command commands!\n");
    return EXIT_FAILURE;
  }

  char *kernel_source = 0;
  int length;
  FILE *f = fopen("vec_add.cl", "rb");

  if (!f) {
    printf("Error: Failed to load vec_add.cl!\n");
    return EXIT_FAILURE;
  }

  fseek(f, 0, SEEK_END);
  length = ftell(f);
  fseek(f, 0, SEEK_SET);
  kernel_source = malloc(length);

  if (!kernel_source) {
    printf(
        "Error: Failed to allocated buffer to load the kernel source code\n");
    return EXIT_FAILURE;
  }

  fread(kernel_source, 1, length, f);
  fclose(f);

  // Create the compute program from the source buffer
  program = clCreateProgramWithSource(context, 1, (const char **)&kernel_source,
                                      NULL, &err);
  if (!program) {
    printf("Error: Failed to create compute program!\n");
    return EXIT_FAILURE;
  }

  // Build the program executable
  err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
  if (err != CL_SUCCESS) {
    size_t len;
    char buffer[2048];

    printf("Error: Failed to build program executable!\n");
    clGetProgramBuildInfo(program, device_id, CL_PROGRAM_BUILD_LOG,
                          sizeof(buffer), buffer, &len);
    printf("%s\n", buffer);
    exit(1);
  }

  // Create the compute kernel in the program we wish to run
  //
  kernel = clCreateKernel(program, "vec_add", &err);
  if (!kernel || err != CL_SUCCESS) {
    printf("Error: Failed to create compute kernel!\n");
    exit(1);
  }

  // Create the input and output arrays in device memory for our calculation
  memory_inp_A = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(int) * COUNT,
                                NULL, NULL);
  memory_inp_B = clCreateBuffer(context, CL_MEM_READ_ONLY, sizeof(int) * COUNT,
                                NULL, NULL);
  memory_out = clCreateBuffer(context, CL_MEM_WRITE_ONLY, sizeof(int) * COUNT,
                              NULL, NULL);
  if (!memory_inp_A || !memory_inp_B || !memory_out) {
    printf("Error: Failed to allocate device memory!\n");
    exit(1);
  }

  // Write our data set into the input array in device memory
  err = clEnqueueWriteBuffer(commands, memory_inp_A, CL_TRUE, 0,
                             sizeof(int) * COUNT, A, 0, NULL, NULL);
  if (err != CL_SUCCESS) {
    printf("Error: Failed to write to source array!\n");
    exit(1);
  }

  err = clEnqueueWriteBuffer(commands, memory_inp_B, CL_TRUE, 0,
                             sizeof(int) * COUNT, B, 0, NULL, NULL);
  if (err != CL_SUCCESS) {
    printf("Error: Failed to write to source array!\n");
    exit(1);
  }

  // Set the arguments to our compute kernel
  err = 0;
  err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &memory_out);
  err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &memory_inp_A);
  err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &memory_inp_B);

  if (err != CL_SUCCESS) {
    printf("Error: Failed to set kernel arguments! %d\n", err);
    exit(1);
  }

  // Get the maximum work group size for executing the kernel on the device
  err = clGetKernelWorkGroupInfo(kernel, device_id, CL_KERNEL_WORK_GROUP_SIZE,
                                 sizeof(local), &local, NULL);
  if (err != CL_SUCCESS) {
    printf("Error: Failed to retrieve kernel work group info! %d\n", err);
    exit(1);
  }

  // Execute the kernel over the entire range of our 1d input data set
  // using the maximum number of work group items for this device
  global = count;
  err = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, &local, 0,
                               NULL, NULL);
  if (err) {
    printf("Error: Failed to execute kernel!\n");
    return EXIT_FAILURE;
  }

  // Wait for the command commands to get serviced before reading back results
  clFinish(commands);

  // Read back the results from the device to verify the output
  err = clEnqueueReadBuffer(commands, memory_out, CL_TRUE, 0,
                            sizeof(float) * count, C, 0, NULL, NULL);
  if (err != CL_SUCCESS) {
    printf("Error: Failed to read output array! %d\n", err);
    exit(1);
  }

  for (int i = 0; i < COUNT; i++) {
    printf("the result of %d + %d is %d\n", A[i], B[i], C[i]);
  }

  printf("executed with group size = %zu\n", local);

  clReleaseMemObject(memory_inp_A);
  clReleaseMemObject(memory_inp_B);
  clReleaseMemObject(memory_out);
  clReleaseProgram(program);
  clReleaseKernel(kernel);
  clReleaseCommandQueue(commands);
  clReleaseContext(context);

  return 0;
}