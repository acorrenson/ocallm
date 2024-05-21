// A simple OpenCL kernel to compute vector addition C = A + B
kernel void vec_add(global int *C, global int *A, global int *B) {
  const int idx = get_global_id(0);
  C[idx] = A[idx] + B[idx];
}