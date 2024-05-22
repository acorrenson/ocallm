// A simple OpenCL kernel to compute vector addition C = A + B
kernel void vec_add(global float *C, global float *A, global float *B) {
  const int idx = get_global_id(0);
  C[idx] = A[idx] + B[idx];
}