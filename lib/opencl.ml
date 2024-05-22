(** OpenCL bindings *)
open Linalg

external test_opencl : string -> string -> unit = "run_kernel_stub"

external vec_add : Vector.t -> Vector.t -> Vector.t -> unit = "vec_add_stub"

external vec_addi :
    (int32, Bigarray.int32_elt, Bigarray.c_layout) Bigarray.Array1.t
  -> (int32, Bigarray.int32_elt, Bigarray.c_layout) Bigarray.Array1.t
  -> (int32, Bigarray.int32_elt, Bigarray.c_layout) Bigarray.Array1.t
  -> unit = "vec_addi_stub"