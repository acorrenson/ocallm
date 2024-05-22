// A simple OpenCL kernel to compute element-wise vector multiplication
kernel void vec_mul(global float *C, global float *A, global float *B) {
  const int idx = get_global_id(0);
  C[idx] = A[idx] * B[idx];
}