(** Algorithmic Differentiation *)

module Forward = struct
  let cross_entropy ~(target : int) ~(logits : Vector.t) : float =
    if target < 0 || target >= Vector.dim logits then
      invalid_arg "cross_entropy: target should be one of the possible class"
    else
      -. log (logits.{target} /. Vector.(logits |> map exp |> sum))

  let embedding ~(class_id : int) ~(weights : Matrix.t) : Vector.t =
    if class_id < 0 || class_id >= Matrix.dim1 weights then
      invalid_arg "cross_entropy: target should be one of the possible class"
    else Matrix.row weights class_id
end

module Backward = struct
  let cross_entropy ~(target : int) ~(logits : Vector.t) : Vector.t =
    let s = Vector.(logits |> map exp |> sum) in
    Vector.mapi (fun i xi ->
      if i = target then (exp xi /. s) -. 1.
      else exp xi /. s
    ) logits

  let embedding ~(class_id : int) ~(weights : Matrix.t) : Matrix.t =
    let height = Matrix.dim1 weights in
    let width  = Matrix.dim2 weights in
    let m = Matrix.zeros ~width ~height in
    for j = 0 to width - 1 do
      m.{class_id, j} <- 1.
    done;
    m
end

(* type nn = {

} *)