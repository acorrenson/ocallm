#include <OpenCL/OpenCL.h>
#include <OpenCL/opencl.h>
#include <caml/bigarray.h>
#include <caml/mlvalues.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static cl_device_id id;            // compute device id
static cl_context context;         // compute context
static cl_uint num_devices;        // number of devices
static cl_command_queue commands;  // compute command queue
static cl_program program_vec_add; // compute program
static cl_program program_vec_mul; // compute program
static cl_kernel kernel_vec_add;   // compute program
static cl_kernel kernel_vec_mul;   // compute program
static size_t vec_add_group_size;  // maximal group size for vec_add
static size_t vec_mul_group_size;  // maximal group size for vec_add

#define Device_val(v) (*((device **)Data_custom_val(v)))

const char *clGetErrorString(int errorCode) {
  switch (errorCode) {
  case 0:
    return "CL_SUCCESS";
  case -1:
    return "CL_DEVICE_NOT_FOUND";
  case -2:
    return "CL_DEVICE_NOT_AVAILABLE";
  case -3:
    return "CL_COMPILER_NOT_AVAILABLE";
  case -4:
    return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
  case -5:
    return "CL_OUT_OF_RESOURCES";
  case -6:
    return "CL_OUT_OF_HOST_MEMORY";
  case -7:
    return "CL_PROFILING_INFO_NOT_AVAILABLE";
  case -8:
    return "CL_MEM_COPY_OVERLAP";
  case -9:
    return "CL_IMAGE_FORMAT_MISMATCH";
  case -10:
    return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
  case -12:
    return "CL_MAP_FAILURE";
  case -13:
    return "CL_MISALIGNED_SUB_BUFFER_OFFSET";
  case -14:
    return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";
  case -15:
    return "CL_COMPILE_PROGRAM_FAILURE";
  case -16:
    return "CL_LINKER_NOT_AVAILABLE";
  case -17:
    return "CL_LINK_PROGRAM_FAILURE";
  case -18:
    return "CL_DEVICE_PARTITION_FAILED";
  case -19:
    return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";
  case -30:
    return "CL_INVALID_VALUE";
  case -31:
    return "CL_INVALID_DEVICE_TYPE";
  case -32:
    return "CL_INVALID_PLATFORM";
  case -33:
    return "CL_INVALID_DEVICE";
  case -34:
    return "CL_INVALID_CONTEXT";
  case -35:
    return "CL_INVALID_QUEUE_PROPERTIES";
  case -36:
    return "CL_INVALID_COMMAND_QUEUE";
  case -37:
    return "CL_INVALID_HOST_PTR";
  case -38:
    return "CL_INVALID_MEM_OBJECT";
  case -39:
    return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
  case -40:
    return "CL_INVALID_IMAGE_SIZE";
  case -41:
    return "CL_INVALID_SAMPLER";
  case -42:
    return "CL_INVALID_BINARY";
  case -43:
    return "CL_INVALID_BUILD_OPTIONS";
  case -44:
    return "CL_INVALID_PROGRAM";
  case -45:
    return "CL_INVALID_PROGRAM_EXECUTABLE";
  case -46:
    return "CL_INVALID_KERNEL_NAME";
  case -47:
    return "CL_INVALID_KERNEL_DEFINITION";
  case -48:
    return "CL_INVALID_KERNEL";
  case -49:
    return "CL_INVALID_ARG_INDEX";
  case -50:
    return "CL_INVALID_ARG_VALUE";
  case -51:
    return "CL_INVALID_ARG_SIZE";
  case -52:
    return "CL_INVALID_KERNEL_ARGS";
  case -53:
    return "CL_INVALID_WORK_DIMENSION";
  case -54:
    return "CL_INVALID_WORK_GROUP_SIZE";
  case -55:
    return "CL_INVALID_WORK_ITEM_SIZE";
  case -56:
    return "CL_INVALID_GLOBAL_OFFSET";
  case -57:
    return "CL_INVALID_EVENT_WAIT_LIST";
  case -58:
    return "CL_INVALID_EVENT";
  case -59:
    return "CL_INVALID_OPERATION";
  case -60:
    return "CL_INVALID_GL_OBJECT";
  case -61:
    return "CL_INVALID_BUFFER_SIZE";
  case -62:
    return "CL_INVALID_MIP_LEVEL";
  case -63:
    return "CL_INVALID_GLOBAL_WORK_SIZE";
  case -64:
    return "CL_INVALID_PROPERTY";
  case -65:
    return "CL_INVALID_IMAGE_DESCRIPTOR";
  case -66:
    return "CL_INVALID_COMPILER_OPTIONS";
  case -67:
    return "CL_INVALID_LINKER_OPTIONS";
  case -68:
    return "CL_INVALID_DEVICE_PARTITION_COUNT";
  case -69:
    return "CL_INVALID_PIPE_SIZE";
  case -70:
    return "CL_INVALID_DEVICE_QUEUE";
  case -71:
    return "CL_INVALID_SPEC_ID";
  case -72:
    return "CL_MAX_SIZE_RESTRICTION_EXCEEDED";
  case -1002:
    return "CL_INVALID_D3D10_DEVICE_KHR";
  case -1003:
    return "CL_INVALID_D3D10_RESOURCE_KHR";
  case -1004:
    return "CL_D3D10_RESOURCE_ALREADY_ACQUIRED_KHR";
  case -1005:
    return "CL_D3D10_RESOURCE_NOT_ACQUIRED_KHR";
  case -1006:
    return "CL_INVALID_D3D11_DEVICE_KHR";
  case -1007:
    return "CL_INVALID_D3D11_RESOURCE_KHR";
  case -1008:
    return "CL_D3D11_RESOURCE_ALREADY_ACQUIRED_KHR";
  case -1009:
    return "CL_D3D11_RESOURCE_NOT_ACQUIRED_KHR";
  case -1010:
    return "CL_INVALID_DX9_MEDIA_ADAPTER_KHR";
  case -1011:
    return "CL_INVALID_DX9_MEDIA_SURFACE_KHR";
  case -1012:
    return "CL_DX9_MEDIA_SURFACE_ALREADY_ACQUIRED_KHR";
  case -1013:
    return "CL_DX9_MEDIA_SURFACE_NOT_ACQUIRED_KHR";
  case -1093:
    return "CL_INVALID_EGL_OBJECT_KHR";
  case -1092:
    return "CL_EGL_RESOURCE_NOT_ACQUIRED_KHR";
  case -1001:
    return "CL_PLATFORM_NOT_FOUND_KHR";
  case -1057:
    return "CL_DEVICE_PARTITION_FAILED_EXT";
  case -1058:
    return "CL_INVALID_PARTITION_COUNT_EXT";
  case -1059:
    return "CL_INVALID_PARTITION_NAME_EXT";
  case -1094:
    return "CL_INVALID_ACCELERATOR_INTEL";
  case -1095:
    return "CL_INVALID_ACCELERATOR_TYPE_INTEL";
  case -1096:
    return "CL_INVALID_ACCELERATOR_DESCRIPTOR_INTEL";
  case -1097:
    return "CL_ACCELERATOR_TYPE_NOT_SUPPORTED_INTEL";
  case -1000:
    return "CL_INVALID_GL_SHAREGROUP_REFERENCE_KHR";
  case -1098:
    return "CL_INVALID_VA_API_MEDIA_ADAPTER_INTEL";
  case -1099:
    return "CL_INVALID_VA_API_MEDIA_SURFACE_INTEL";
  case -1100:
    return "CL_VA_API_MEDIA_SURFACE_ALREADY_ACQUIRED_INTEL";
  case -1101:
    return "CL_VA_API_MEDIA_SURFACE_NOT_ACQUIRED_INTEL";
  default:
    return "CL_UNKNOWN_ERROR";
  }
}

void error(char *msg) {
  fprintf(stderr, "%s\n", msg);
  exit(EXIT_FAILURE);
}

char *read_file(const char *path) {
  int length;
  FILE *f = fopen(path, "rb");

  if (!f) {
    error("Error: Failed to load kernel!");
  }

  fseek(f, 0, SEEK_END);
  length = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *data = malloc(length);

  fread(data, 1, length, f);
  fclose(f);
  return data;
}

void initialize() {
  int err;
  err = clGetDeviceIDs(NULL, CL_DEVICE_TYPE_ALL, 1, &id, &num_devices);
  if (err != CL_SUCCESS) {
    error("Error: Failed to create a device group!");
  }

  // Create a compute context
  context = clCreateContext(NULL, num_devices, &id, NULL, NULL, &err);
  if (!context) {
    error("Error: Failed to create a compute context!");
  }

  // Create a command commands
  commands = clCreateCommandQueue(context, id, 0, &err);
  if (!commands) {
    error("Error: Failed to create a command queue!");
  }

  printf("OpenCL initialized!\n");
}

void load_kernels() {
  char *kernel_source_add;
  char *kernel_source_mul;
  int err;

  printf("loading kernels\n");

  // Create program vec_add
  kernel_source_add = read_file("./myopencl/kernels/vec_add.cl");
  program_vec_add = clCreateProgramWithSource(
      context, 1, (const char **)&kernel_source_add, NULL, &err);
  if (!program_vec_add || err != CL_SUCCESS) {
    error("Error: Failed to create compute program vec_add!");
  }

  printf("Kernel add loaded!\n");

  // Create program vec_mul
  kernel_source_mul = read_file("./myopencl/kernels/vec_mul.cl");
  printf("Kernel mul loaded!\n");
  program_vec_mul = clCreateProgramWithSource(
      context, 1, (const char **)&kernel_source_mul, NULL, &err);
  if (!program_vec_mul || err != CL_SUCCESS) {
    error("Error: Failed to create compute program vec_mul!");
  }

  size_t len;
  char buffer[2048];

  // Build program vec_add
  err = clBuildProgram(program_vec_add, 0, NULL, NULL, NULL, NULL);
  if (err != CL_SUCCESS) {
    clGetProgramBuildInfo(program_vec_add, id, CL_PROGRAM_BUILD_LOG,
                          sizeof(buffer), buffer, &len);
    printf("%s\n", buffer);
    error("Error: Failed to build program vec_add!");
  }

  // Build program vec_mul
  err = clBuildProgram(program_vec_mul, 0, NULL, NULL, NULL, NULL);
  if (err != CL_SUCCESS) {
    clGetProgramBuildInfo(program_vec_mul, id, CL_PROGRAM_BUILD_LOG,
                          sizeof(buffer), buffer, &len);
    printf("%s\n", buffer);
    error("Error: Failed to build program vec_mul!");
  }

  // Create the compute kernel vec_add
  kernel_vec_add = clCreateKernel(program_vec_add, "vec_add", &err);
  if (!kernel_vec_add || err != CL_SUCCESS) {
    error("Error: Failed to create compute kernel vec_add!");
  }

  // Create the compute kernel vec_mul
  kernel_vec_mul = clCreateKernel(program_vec_mul, "vec_mul", &err);
  if (!kernel_vec_mul || err != CL_SUCCESS) {
    error("Error: Failed to create compute kernel vec_mul!");
  }

  err = clGetKernelWorkGroupInfo(kernel_vec_add, id, CL_KERNEL_WORK_GROUP_SIZE,
                                 sizeof(vec_add_group_size),
                                 &vec_add_group_size, NULL);
  if (err != CL_SUCCESS) {
    error("Error: Failed to retrieve kernel work group info for vec_add!\n");
  }

  err = clGetKernelWorkGroupInfo(kernel_vec_mul, id, CL_KERNEL_WORK_GROUP_SIZE,
                                 sizeof(vec_mul_group_size),
                                 &vec_mul_group_size, NULL);
  if (err != CL_SUCCESS) {
    error("Error: Failed to retrieve kernel work group info for vec_mul!\n");
  }

  printf("Kernel loaded!\n");
  free(kernel_source_add);
  free(kernel_source_mul);
}

void opencl_init() {
  initialize();
  load_kernels();
}

#define VEC_ADD 0
#define VEC_MUL 1

void vec_bop(int op, float *a, float *b, float *c, int dim) {
  clock_t start = clock();
  cl_int err, err_a, err_b;
  size_t size = sizeof(float) * dim;
  cl_mem memory_A = clCreateBuffer(context, CL_MEM_READ_ONLY, size, NULL, NULL);
  cl_mem memory_B = clCreateBuffer(context, CL_MEM_READ_ONLY, size, NULL, NULL);
  cl_mem memory_C =
      clCreateBuffer(context, CL_MEM_WRITE_ONLY, size, NULL, NULL);
  if (!memory_A || !memory_B || !memory_C) {
    error("Error: Failed to allocate device memory!");
  }
  err_a = clEnqueueWriteBuffer(commands, memory_A, CL_TRUE, 0, size, a, 0, NULL,
                               NULL);
  err_b = clEnqueueWriteBuffer(commands, memory_B, CL_TRUE, 0, size, b, 0, NULL,
                               NULL);

  if (err_a != CL_SUCCESS || err_b != CL_SUCCESS) {
    error("Error: Failed to write to source arrays!");
  }

  cl_kernel kernel;
  size_t local;
  size_t global;

  if (op == VEC_ADD) {
    kernel = kernel_vec_add;
    local = vec_add_group_size;
  } else if (op == VEC_MUL) {
    kernel = kernel_vec_mul;
    local = vec_mul_group_size;
  } else {
    error("Invalid binary operation");
  }

  err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &memory_C);
  err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &memory_A);
  err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &memory_B);

  if (err != CL_SUCCESS) {
    error("Error: Failed to set kernel arguments!");
  }

  // Execute the kernel over the entire range of our 1d input data set
  // using the maximum number of work group items for this device
  global = dim;
  local = dim < local ? dim : local;
  err = clEnqueueNDRangeKernel(commands, kernel, 1, NULL, &global, &local, 0,
                               NULL, NULL);
  if (err) {
    printf("ClError: %s\n", clGetErrorString(err));
    error("Error: Failed to execute kernel!");
  }

  // Wait for the command commands to get serviced before reading back results
  clFinish(commands);

  // Read back the results from the device to verify the output
  err = clEnqueueReadBuffer(commands, memory_C, CL_TRUE, 0, size, c, 0, NULL,
                            NULL);
  if (err != CL_SUCCESS) {
    error("Error: Failed to copy output array!");
  }

  clReleaseMemObject(memory_A);
  clReleaseMemObject(memory_B);
  clReleaseMemObject(memory_C);
  clReleaseCommandQueue(commands);

  clock_t stop = clock();
  double elapsed = (float)(stop - start) / CLOCKS_PER_SEC;
  printf("actual compute time = %f\n", elapsed);
}

void vec_add(float *a, float *b, float *c, int dim) {
  vec_bop(VEC_ADD, a, b, c, dim);
}

void vec_mul(float *a, float *b, float *c, int dim) {
  vec_bop(VEC_MUL, a, b, c, dim);
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