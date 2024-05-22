#include <OpenCL/OpenCL.h>
#include <OpenCL/opencl.h>
#include <caml/bigarray.h>
#include <caml/mlvalues.h>
#include <stddef.h>
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

  size_t max_work_item_sizes[3] = {0, 0, 0};
  clGetDeviceInfo(dev.id, CL_DEVICE_MAX_WORK_ITEM_SIZES, sizeof(size_t) * 3,
                  max_work_item_sizes, NULL);
  size_t max_work_item_size = max_work_item_sizes[0];
  printf("max work item size is %zu\n", max_work_item_size);
  printf("max local work size is %zu\n", local);

  // CL_DEVICE_MAX_WORK_ITEM_SIZES

  // Execute the kernel over the entire range of our 1d input data set
  // using the maximum number of work group items for this device
  global = dim;
  local = dim < local ? dim : local;
  err = clEnqueueNDRangeKernel(dev.commands, dev.kernel, 1, NULL, &global,
                               &local, 0, NULL, NULL);
  if (err) {
    printf("ClError: %s\n", clGetErrorString(err));
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