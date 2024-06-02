open Ocallm

let _time_it (f : unit -> unit) : float =
  let t = Sys.time () in
  f ();
  Sys.time () -. t

let get_sample () =
  let i = Random.int 30000 in
  Mnist.of_single_file (Printf.sprintf "./data/mnist/digit_%d.csv" i)

let estimate_loss nn =
  let n = 10 in
  Vector.init n (fun _ ->
    let x = get_sample () in
    Nn.NN.loss_eager nn x (Nn.Loss.Class x.label)
  )
  |> Vector.mean
  |> Printf.printf "current loss (average over %d random sample) = %1.10f\n" n

let mnist () =
  let open Nn in
  Random.self_init ();
  let nn_dim = 28 * 28 + 1 in
  let add_bias x = Array.to_list x |> List.cons 1 |> Array.of_list in
  let normalize x = Array.map (fun x -> (float_of_int x) /. 255.) x in
  let nn_pre s = s.Mnist.data |> add_bias |> normalize |> Vector.of_array in
  let nn_lay = [ Layer.relu 10 ] in
  let nn_loss = Loss.CROSS_ENTROPY in
  let nn_post = Vector.arg_max in
  let nn : (Mnist.t, int) NN.t =
    NN.make nn_pre nn_dim nn_lay nn_loss nn_post
  in
  
  for i = 0 to 100000 do
    let x = get_sample () in
    let y = Loss.Class x.label in
    NN.forward nn x;
    NN.backward nn x y;
    if i mod 1000 = 0 then begin
      estimate_loss nn;
      flush_all ()
    end
  done

let () =
  if Array.length Sys.argv < 2 then
    failwith "usage: ocallm (sanitize | mnist)"
  else
    match Sys.argv.(1) with
    | "sanitize" -> Wikitext.sanitize "./data/articles"
    | "mnist" ->
      mnist ()
    | _ -> failwith "usage: ocallm (sanitize | mnist)"
