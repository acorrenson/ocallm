(** {1 Fast Algebra Based on OpenCL} *)

let[@inline] fail_if test msg =
  if test then failwith msg

(* external init : unit -> unit = "opencl_init" *)

(* external vec_add : Vector.t -> Vector.t -> Vector.t -> unit = "vec_add_stub"

external vec_mul : Vector.t -> Vector.t -> Vector.t -> unit = "vec_mul_stub"

let vec_dot (v1 : Vector.t) (v2 : Vector.t) : float =
  let dim = Bigarray.Array1.dim v1 in
  let aux = Vector.create dim in
  vec_mul v1 v2 aux;
  let res = ref 0. in
  for i = 0 to dim - 1 do
    res := !res +. aux.{i}
  done;
  !res *)

let vec_dot (v1 : Vector.t) (v2 : Vector.t) : float =
  let dim = Bigarray.Array1.dim v1 in
  let res = ref 0. in
  for i = 0 to dim - 1 do
    res := !res +. v1.{i} *. v2.{i}
  done;
  !res

let slow_vec_mul (v1 : Vector.t) (v2 : Vector.t) (v3 : Vector.t) : unit =
  let dim = Bigarray.Array1.dim v1 in
  for i = 0 to dim - 1 do
    v3.{i} <- v1.{i} *. v2.{i}
  done

let slow_vec_add (v1 : Vector.t) (v2 : Vector.t) (v3 : Vector.t) : unit =
  let dim = Bigarray.Array1.dim v1 in
  for i = 0 to dim - 1 do
    v3.{i} <- v1.{i} +. v2.{i}
  done

let mat_mul (_m1 : Matrix.t) (_m2 : Matrix.t) : float =
  failwith "todo"

let mat_vec_mul (m : Matrix.t) (v : Vector.t) : Vector.t =
  let i_n = Bigarray.Array2.dim1 m in
  let k_n = Bigarray.Array2.dim2 m in
  fail_if (Bigarray.Array1.dim v <> k_n) "app: incompatible dimensions";
  let res = Vector.create i_n in
  for i = 0 to i_n - 1 do
    res.{i} <- vec_dot (Matrix.row m i) v;
  done;
  res

let mat_sub (m1 : Matrix.t) (m2 : Matrix.t) : Matrix.t =
  assert (Matrix.dim1 m1 = Matrix.dim1 m2);
  assert (Matrix.dim2 m1 = Matrix.dim2 m2);
  Matrix.init ~height:(Matrix.dim1 m1) ~width:(Matrix.dim2 m1) (fun i j ->
    m1.{i, j} -. m2.{i, j}
  )

let scale (c : float) (m : Matrix.t) : Matrix.t =
  Matrix.init ~height:(Matrix.dim1 m) ~width:(Matrix.dim2 m) (fun i j ->
    c *. m.{i, j}
  )

(** Multiply a vector with a transposed vector (i.e. [vec_vec_t_mul v1 v2 = v1 . v2^t]) *)
let vec_vec_t_mul (v1 : Vector.t) (v2 : Vector.t) =
  let width = Vector.dim v2 in
  let height = Vector.dim v1 in
  let m = Matrix.create ~width ~height in
  for i = 0 to height - 1 do
    for j = 0 to width - 1 do
      m.{i, j} <- v1.{i} *. v2.{j}
    done
  done;
  m

let hadamar (v1 : Vector.t) (v2 : Vector.t) =
  assert (Vector.dim v1 = Vector.dim v2);
  Vector.init (Vector.dim v1) (fun i -> v1.{i} *. v2.{i})

(** Multiply the transposed of a matrix with a vector (i.e. [mat_t_vec_mul m v = m^t . v]) *)
let mat_t_vec_mul (m : Matrix.t) (v : Vector.t) =
  assert (Matrix.dim1 m = Vector.dim v);
  Vector.init (Matrix.dim2 m) (fun i ->
    let r = ref 0. in
    for j = 0 to Matrix.dim1 m - 1 do
      r := !r +. v.{j} *. m.{j, i}
    done;
    !r
  )