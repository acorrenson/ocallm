(** {1 Neural Network Library} *)

(** Useful (scalar) functions and their derivatives *)
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

(** Activation functions *)
module Activation = struct
  type t = RELU | SIGMOID

  (** Apply activation to a vector (compute [a = sigma(z)]) *)
  let activate (act : t) (z : Vector.t) =
    match act with
    | RELU -> Vector.map Functions.relu z
    | SIGMOID -> Vector.map Functions.sigmoid z

  (** Apply the derivative of the activation to a vector (compute [sigma'(z)] *)
  let activate' (act : t) (z : float) =
    match act with
    | RELU -> Functions.relu' z
    | SIGMOID -> Functions.sigmoid' z

  let pp fmt (act : t) =
    match act with
    | RELU -> Format.fprintf fmt "RELU"
    | SIGMOID -> Format.fprintf fmt "SIGMOID"
end

(** Representation of layers of a network *)
module Layer = struct
  type t = {
    nodes : int;                (** number of nodes*)
    activation : Activation.t;  (** activation function *)
  }

  (** create a relu layer *)
  let relu (nodes : int) : t =
    { nodes; activation = RELU }

  (** create a sigmoid layer *)
  let sigmoid (nodes : int) : t =
    { nodes; activation = SIGMOID }

  (** Feed a vector [x] (from the previous layer)
      into the current layer [lay], assuming weights of [lay] are [w].
      Returns a tuple [(z, a)] where [z = w.x] and [a = activation(z)].
  *)
  let forward_za (lay : t) (w : Matrix.t) (x : Vector.t) =
    let z = Linalg.mat_vec_mul w x in
    let a = Activation.activate lay.activation z in
    (z, a)

  (** Feed a vector [x] (from the previous layer)
      into the current layer [lay], assuming weights of [lay] are [w].
  *)
  let forward (lay : t) (w : Matrix.t) (x : Vector.t) =
    snd (forward_za lay w x)

  (** Compute the deltas of the current layer as a function of the weights
      [w] of the next layer, the deltas [d] of the next layer, and the logits [z]
      of the current layer
  *)
  let backward (lay : t) (z : Vector.t) (w : Matrix.t) (d : Vector.t) =
    let z' = Vector.map (Activation.activate' lay.activation) z in
    Linalg.(hadamar z' (mat_t_vec_mul w d))

  let pp fmt (lay : t) =
    Format.fprintf fmt "(%d => %a)" lay.nodes Activation.pp lay.activation
end

module Loss = struct
  (*
    - MSE expects the last layer to have dimension [1]
    - CROSS_ENTROPY expects the last layer to have dimension [n] where [n] is a number of classes
  *)
  type t = MSE | CROSS_ENTROPY

  type target =
    | Class of int
    | Vec of Vector.t

  let softmax (x : Vector.t) =
    let y = Vector.map exp x in
    let s = Vector.sum y in
    for i = 0 to Vector.dim y - 1 do
      y.{i} <- y.{i} /. s
    done;
    y

  (** Computes the loss as a function of logits *)
  let forward (loss : t) (logits : Vector.t) (target : target) =
    match loss with
    | MSE -> failwith "MSE not implemented"
    | CROSS_ENTROPY ->
      match target with
      | Class c ->
        Vector.(logits |> map exp |> sum |> log) -. logits.{c}
      | Vec _ -> failwith "only classes are supported as targets"

  (** Directional derivatives of the loss (w.r.t logits) *)
  let backward (loss : t) (logits : Vector.t) (target : target) : Vector.t =
    match loss with
    | MSE -> failwith "MSE not implemented"
    | CROSS_ENTROPY ->
      match target with
      | Class c ->
        let v = softmax logits in
        v.{c} <- v.{c} -. 1.; v
      | Vec _ -> failwith "only classes are supported as targets"

end

(** Neural Networks *)
module NN = struct
  type ('i, 'o) t = {
    pre    : 'i -> Vector.t;  (** How to transform inputs into vectors *)
    inputs : int;
    layers : Layer.t array;   (** Layers of the network *)
    loss   : Loss.t;          (** How to evaluate the error of the model *)
    post   : Vector.t -> 'o;  (** How to transform outputs into predictions *)

    weights : Matrix.t array; (** Current weights (per layer) *)
    _Z : Vector.t array;      (** Current outputs (per layer) *)
    _A : Vector.t array;      (** Current activations (per layer) *)
    _D : Vector.t array;      (** Current deltas (per layer)*)
  }

  let make_simple (inputs : int) (l : Layer.t list) =
    let nb_layers = List.length l in
    let pre = Fun.id in
    let layers = Array.of_list l in
    let loss = Loss.CROSS_ENTROPY in
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

  (** Feed an input [x] to a model [nn] (precomputes outputss & activations of all layers) *)
  let forward (nn : ('i, 'o) t) (x : 'i) : unit =
    let nb_layers = Array.length nn.layers in
    let (z0, a0) = Layer.forward_za nn.layers.(0) nn.weights.(0) (nn.pre x) in
    nn._Z.(0) <- z0;
    nn._A.(0) <- a0;
    for i = 1 to nb_layers - 1 do
      let (zi, ai) = Layer.forward_za nn.layers.(i) nn.weights.(i) nn._A.(i - 1) in
      nn._Z.(i) <- zi;
      nn._A.(i) <- ai;
    done

  (** Feed an input [x] to a model [nn] and outputs a final prediction (without computing/storing all intermediate results) *)
  let predict_eager (nn : ('i, 'o) t) (x : 'i) : 'o =
    let v = ref (nn.pre x) in
    let nb_layers = Array.length nn.layers in
    for i = 0 to nb_layers - 1 do
      v := Layer.forward nn.layers.(i) nn.weights.(i) !v
    done;
    nn.post (!v)

  (** Feed an input [x] to a model [nn] and outputs a final prediction (computes and stores all intermediate results) *)
  let predict (nn : ('i, 'o) t) (x : 'i) : 'o =
    let i_last = Array.length nn.layers - 1 in
    forward nn x;
    nn.post nn._A.(i_last)

  (** Backpropagate the error made when target should be [target] *)
  let backward (nn : ('i, 'o) t) (x : 'i) (target : Loss.target) : unit =
    let lr = exp (-. 3.) in
    let nb_layers = Array.length nn.layers in
    let i_last = nb_layers - 1 in
    let logits = nn._A.(i_last) in
    
    (* Compute the deltas *)
    nn._D.(i_last) <- Loss.backward nn.loss logits target;
    for i = i_last - 1 downto 0 do
      nn._D.(i) <- Layer.backward nn.layers.(i) nn._Z.(i) nn.weights.(i + 1) nn._D.(i + 1);
      assert (Vector.dim nn._D.(i) = nn.layers.(i).nodes);
    done;

    (* Update the weights *)
    nn.weights.(0) <- Linalg.(mat_sub nn.weights.(0) (scale lr (vec_vec_t_mul nn._D.(0) (nn.pre x))));
    for i = 1 to i_last do
      nn.weights.(i) <- Linalg.(mat_sub nn.weights.(i) (scale lr (vec_vec_t_mul nn._D.(i) nn._A.(i - 1))));
    done

  let loss (nn : ('i, 'o) t) (target : Loss.target) : float =
    let i_last = Array.length nn.layers - 1 in
    Loss.forward nn.loss (nn._A.(i_last)) target

  let pp fmt nn =
    let nb_layers = Array.length nn.layers in
    Format.fprintf fmt "%d-" nn.inputs;
    Array.iteri (fun i lay ->
      if i = nb_layers - 1 then
        Format.fprintf fmt "%a" Layer.pp lay
      else
        Format.fprintf fmt "%a-" Layer.pp lay
    ) nn.layers
end

let test () =
  let nn = NN.make_simple 2 [ Layer.relu 2; Layer.relu 3; Layer.sigmoid 2 ] in
  Format.printf "%a\n" NN.pp nn;
  let x = Vector.of_array [| 1.; 2. |] in
  let y = Loss.Class 0 in
  for _ = 0 to 50 do
    NN.forward nn x;
    Printf.printf "current loss = %1.10f\n" (NN.loss nn y);
    NN.backward nn x y
  done

  (* for i = 0 to 2 do

    Format.printf "Deltas of layer %d are:\n%a" i Vector.pp nn._D.(i)
  done
 *)
