(** {1 Basic Linear Algebra Stuff} *)

let[@inline] fail_if test msg =
  if test then failwith msg

(** Vectors based on OCaml [Bigarray] *)
module Vector = struct
  open Bigarray

  type t = (float, float32_elt, c_layout) Array1.t

  let of_array arr : t =
    Array1.of_array float32 c_layout arr

  let create size : t =
    Array1.create float32 c_layout size

  let zeros size : t =
    Array1.init float32 c_layout size (fun _ -> 0.)

  let ones size : t =
    Array1.init float32 c_layout size (fun _ -> 1.)

  let dot (v1 : t) (v2 : t) : float =
    let size = Array1.dim v1 in
    fail_if (size <> Array1.dim v2) "dot: incompatible dimensions";
    Array.init size (fun i -> v1.{i} *. v2.{i})
    |> Array.fold_left Float.add 0.
end

(** Matrices based on OCaml [Bigarray] *)
module Matrix = struct
  open Bigarray

  type t = (float, float32_elt, c_layout) Array2.t

  let of_array arr : t =
    Array2.of_array float32 c_layout arr

  let to_array m =
    Array.init (Array2.dim1 m) (fun i ->
      Array.init (Array2.dim2 m) (fun j -> m.{i, j})
    )

  let create ~width ~height : t =
    Array2.create float32 c_layout height width

  let zeros ~width ~height : t =
    Array2.init float32 c_layout height width (fun _ _ -> 0.)

  let ones ~width ~height : t =
    Array2.init float32 c_layout height width (fun _ _ -> 1.)

  let mul (m1 : t) (m2 : t) : t =
    let height = Array2.dim1 m1 in
    let width  = Array2.dim2 m2 in
    let common = Array2.dim2 m1 in
    fail_if (Array2.dim1 m2 <> common) "mul: incompatible dimensions";
    let res = create ~width ~height in
    for i = 0 to height - 1 do
      for j = 0 to width - 1 do
        res.{i, j} <- 0.;
        for k = 0 to common - 1 do
          res.{i, j} <- res.{i, j} +. m1.{i, k} *. m2.{k, j}
        done
      done
    done;
    res
end

let app (m : Matrix.t) (v : Vector.t) : Vector.t =
  let size   = Bigarray.Array2.dim1 m in
  let common = Bigarray.Array2.dim2 m in
  fail_if (Bigarray.Array1.dim v <> common) "app: incompatible dimensions";
  let res = Vector.create size in
  for i = 0 to size - 1 do
    res.{i} <- 0.;
    for j = 0 to common - 1 do
      res.{i} <- res.{i} +. m.{i, j} *. v.{j}
    done
  done;
  res