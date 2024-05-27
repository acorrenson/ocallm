module Functions = struct
  let relu (x : float) =
    max 0. x
  
  let relu' (x : float) =
    if x <= 0. then 0. else 1.
  
  let sigmoid (x : float) =
    1. /. (1. +. exp (-. x))
  
  let sigmoid' (x : float) =
    sigmoid x *. (1. -. sigmoid x)
end

module Activation = struct
  type t = RELU | SIGMOID

  let activate (a : t) (x : Vector.t) =
    match a with
    | RELU -> Vector.map Functions.relu x
    | SIGMOID -> Vector.map Functions.sigmoid x

  let activate' (a : t) (x : float) =
    match a with
    | RELU -> Functions.relu' x
    | SIGMOID -> Functions.sigmoid' x

  let pp fmt (act : t) =
    match act with
    | RELU -> Format.fprintf fmt "RELU"
    | SIGMOID -> Format.fprintf fmt "SIGMOID"
end

module Layer = struct
  type t = {
    nodes : int;
    activation : Activation.t;
  }

  let relu (nodes : int) : t =
    { nodes; activation = RELU }

  let sigmoid (nodes : int) : t =
    { nodes; activation = SIGMOID }

  let forward (lay : t) (w : Matrix.t) (x : Vector.t) =
    Activation.activate lay.activation (Linalg.mat_vec_mul w x)

  let pp fmt (lay : t) =
    Format.fprintf fmt "(%d => %a)" lay.nodes Activation.pp lay.activation
end

module Loss = struct
  (*
    - MSE expects the last layer to have dimension [1]
    - CROSS_ENTROPY expects the last layer to have dimension [n] where [n] is a number of classes
  *)
  type t = MSE | CROSS_ENTROPY
end

module NN = struct
  type ('i, 'o) t = {
    pre    : 'i -> Vector.t;  (** How to transform inputs into vectors *)
    inputs : int;
    layers : Layer.t array;   (** Layers of the network *)
    loss   : Loss.t;          (** How to evaluate the error of the model *)
    post   : Vector.t -> 'o;  (** How to transform outputs into predictions *)

    weights : Matrix.t array; (** Current weights *)
    _Z : Vector.t array;      (** Current outputs *)
    _A : Vector.t array;      (** Current activations *)
    _D : Vector.t array;      (** Current deltas *)
  }

  let make_simple (inputs : int) (l : Layer.t list) =
    let nb_layers = List.length l in
    let pre = Fun.id in
    let layers = Array.of_list l in
    let loss = Loss.MSE in
    let post = Fun.id in
    let weights = Array.init nb_layers (fun i ->
      let width = if i = 0 then inputs else layers.(i - 1).nodes in
      let height = layers.(i).nodes in
      Matrix.random ~width ~height
    ) in
    let _Z = Array.init nb_layers (fun i -> Vector.create layers.(i).nodes) in
    let _A = Array.init nb_layers (fun i -> Vector.create layers.(i).nodes) in
    let _D = Array.init nb_layers (fun i -> Vector.create layers.(i).nodes) in
    { pre; inputs; layers; loss; post; weights; _Z; _A; _D }

  let activate nn i =
    Activation.activate nn.layers.(i).activation

  let forward (nn : ('i, 'o) t) (x : 'i) : unit =
    let nb_layers = Array.length nn.layers in
    nn._A.(0) <- Layer.forward nn.layers.(0) nn.weights.(0) (nn.pre x);
    for i = 1 to nb_layers - 1 do
      nn._A.(i) <- Layer.forward nn.layers.(i) nn.weights.(i) nn._A.(i - 1)
    done

  let predict_eager (nn : ('i, 'o) t) (x : 'i) : 'o =
    let v = ref (nn.pre x) in
    let nb_layers = Array.length nn.layers in
    for i = 0 to nb_layers - 1 do
      v := Layer.forward nn.layers.(i) nn.weights.(i) !v
    done;
    nn.post (!v)

  let predict (nn : ('i, 'o) t) (x : 'i) : 'o =
    let i_last = Array.length nn.layers - 1 in
    forward nn x;
    nn.post nn._A.(i_last)
end

let test () =
  let nn = NN.make_simple 2 [ Layer.relu 2; Layer.relu 3; Layer.sigmoid 2 ] in
  let x = Vector.of_array [| 1.; 2. |] in
  let p1 = NN.predict nn x in
  let p2 = NN.predict_eager nn x in
  Array.iteri (fun i v ->
    let len = Vector.dim v in
    for j = 0 to len - 1 do
      Printf.printf "A_%d[%d] = %f\n" i j nn._A.(i).{j}
    done
  ) nn._A;
  let len = Vector.dim p2 in
  for j = 0 to len - 1 do
    Printf.printf "P[%d] = %f\n" j p2.{j}
  done;
  assert (p1 = p2)
