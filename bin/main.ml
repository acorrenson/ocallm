open Ocallm
(* open Linalg *)

let src = "./data/articles"

let complete_file_name dir file =
  Printf.sprintf "%s/%s/%s" src dir file

let complete_path dir =
  Printf.sprintf "%s/%s" src dir

let fail_and_usage () =
  Printf.eprintf "usage: %s [sanitize]\n" Sys.argv.(0);
  exit (-1)

let sanitize_dir dir =
  Printf.printf "processing [%s]\n" dir;
  Array.iter (fun file ->
    if Filename.extension file = ".txt" then begin
      let fname = complete_file_name dir file in
      Printf.printf "   sanitizing %s " fname;
      if Wikitext.Sanitizer.sanitize fname then
        print_string "done ✅\n"
      else
        print_string "failed ❌\n"
    end
  ) (Sys.readdir (complete_path dir))

let sanitize () =
  Array.iter (fun dir ->
    if String.starts_with ~prefix:"batch_" dir then
      sanitize_dir dir
    else ()
  ) (Sys.readdir src)

let _time_it (f : unit -> unit) : float =
  let t = Sys.time () in
  f ();
  Sys.time () -. t

let mnist () =
  let open Nn in
  Random.self_init ();
  let nn_dim = 28 * 28 + 1 in
  let nn_pre s = s.Mnist.data |> Array.to_list |> List.cons 1 |> Array.of_list |> Array.map Float.of_int |> Vector.of_array in
  let nn_lay = [ Layer.sigmoid nn_dim ] in
  let nn_loss = Loss.CROSS_ENTROPY in
  let nn_post = Vector.arg_max in
  Printf.printf "Creating the NN\n";
  flush_all ();
  let nn : (Mnist.t, int) NN.t =
    NN.make nn_pre nn_dim nn_lay nn_loss nn_post
  in
  let get_sample () =
    let i = Random.int 30000 in
    (* Printf.printf "sampled digit nb %d\n" i;
    flush_all (); *)
    Mnist.of_single_file (Printf.sprintf "./data/mnist/digit_%d.csv" i)
  in
  Printf.printf "Starting to learn\n";
  flush_all ();
  for i = 0 to 10000 do
    let x = get_sample () in
    let y = Loss.Class (x.Mnist.label) in
    NN.forward nn x;
    NN.backward nn x y;
    if i mod 100 = 0 then begin
      Printf.printf "current loss = %1.10f [%1.10f]\n" (NN.loss nn y) (NN.loss_eager nn x y);
      flush_all ()
    end
  done

let () =
  if Array.length Sys.argv < 2 then
    fail_and_usage ()
  else
    match Sys.argv.(1) with
    | "sanitize" -> sanitize ()
    | "mnist" ->
      Printf.printf "Hello\n";
      flush_all ();
      mnist ()
    | _ -> fail_and_usage ()
