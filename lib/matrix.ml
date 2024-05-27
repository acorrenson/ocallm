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

let random ~width ~height : t =
  Array2.init float32 c_layout height width (fun _ _ -> Random.float 1.)

let zeros ~width ~height : t =
  Array2.init float32 c_layout height width (fun _ _ -> 0.)

let ones ~width ~height : t =
  Array2.init float32 c_layout height width (fun _ _ -> 1.)

let row (m : t) (i : int) : Vector.t =
  Array2.slice_left m i

let dim1 (m : t) : int =
  Array2.dim1 m

let dim2 (m : t) : int =
  Array2.dim2 m