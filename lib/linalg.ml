(** {1 Fast Algebra Based on OpenCL} *)

let[@inline] fail_if test msg =
  if test then failwith msg

external vec_add : Vector.t -> Vector.t -> Vector.t -> unit = "vec_add_stub"

external vec_mul : Vector.t -> Vector.t -> Vector.t -> unit = "vec_mul_stub"

let vec_dot (v1 : Vector.t) (v2 : Vector.t) : float =
  let dim = Bigarray.Array1.dim v1 in
  let aux = Vector.create dim in
  vec_mul v1 v2 aux;
  let res = ref 0. in
  for i = 0 to dim - 1 do
    res := !res +. aux.{i}
  done;
  !res

let mat_mul (m1 : Matrix.t) (m2 : Matrix.t) : float =
  failwith "todo"

let mat_vec_mul (m : Matrix.t) (v : Vector.t) : Vector.t =
  let rows = Bigarray.Array2.dim1 m in
  let common = Bigarray.Array2.dim2 m in
  fail_if (Bigarray.Array1.dim v <> common) "app: incompatible dimensions";
  let res = Vector.create rows in
  for i = 0 to rows - 1 do
    res.{i} <- vec_dot (Matrix.row m i) v;
  done;
  res