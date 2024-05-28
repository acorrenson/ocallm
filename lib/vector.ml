open Bigarray

type t = (float, float32_elt, c_layout) Array1.t

let of_array arr : t =
  Array1.of_array float32 c_layout arr

let create size : t =
  Array1.create float32 c_layout size

let zeros size : t =
  Array1.init float32 c_layout size (fun _ -> 0.)

let random size : t =
    Array1.init float32 c_layout size (fun _ -> Random.float max_float)

let ones size : t =
  Array1.init float32 c_layout size (fun _ -> 1.)

let dim (x : t) : int =
  Array1.dim x

let map (f : float -> float) (x : t) : t =
  Array1.init float32 c_layout (dim x) (fun i -> f x.{i})

let mapi (f : int -> float -> float) (x : t) : t =
  Array1.init float32 c_layout (dim x) (fun i -> f i x.{i})

let init (dim : int) (f : int -> float) : t =
  Array1.init float32 c_layout dim (fun i -> f i)

let sum (x : t) : float =
  let s = ref 0. in
  for i = 0 to pred (dim x) do
    s := !s +. x.{i}
  done;
  !s

let pp fmt (v : t) : unit =
  Format.fprintf fmt "Vector\n";
  for i = 0 to dim v - 1 do
    Format.fprintf fmt "  %1.3f\n" v.{i}
  done

