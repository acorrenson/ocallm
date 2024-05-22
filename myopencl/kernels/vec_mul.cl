// A simple OpenCL kernel to compute element-wise vector multiplication
kernel void vec_mul(global int *C, global int *A, global int *B) {
  const int idx = get_global_id(0);
  C[idx] = A[idx] * B[idx];
}