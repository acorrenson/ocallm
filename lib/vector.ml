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