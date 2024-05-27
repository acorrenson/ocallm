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
  

(* let stress_test () =
  let n = 1 lsl 22 in
  let a = Vector.random n in
  let b = Vector.random n in
  let c = Vector.zeros n in
  Printf.printf "CPU    runtime: %f\n" (time_it (fun () -> ignore(Linalg.slow_vec_mul a b c)));
  Printf.printf "OpenCL runtime: %f\n" (time_it (fun () -> ignore(Linalg.vec_mul a b c))) *)

let () =
  if Array.length Sys.argv < 2 then
    fail_and_usage ()
  else
    match Sys.argv.(1) with
    | "sanitize" -> sanitize ()
    (* | "opencl" ->
      Linalg.init ();
      let c = Vector.of_array [| 0.; 0.; 0.; 0.; |] in
      let a = Vector.of_array [| 1.; 2.; 3.; 4.; |] in
      let b = Vector.of_array [| 3.; 4.; 5.; 6.; |] in
      Linalg.vec_add a b c;
      for i = 0 to 3 do
        Printf.printf "%f +. %f = %f\n" a.{i} b.{i} c.{i}
      done;
      let r = Linalg.vec_dot a b in
      Printf.printf "dot product is %f\n" r;
      let m = Matrix.of_array [| [| 1.; 0. |]; [| 0.; 2.|]|] in
      let v = Vector.of_array [| 2.; 3.|] in
      let p = Linalg.mat_vec_mul m v in
      Printf.printf "matrix vector product is [%f %f]\n" p.{0} p.{1}
    | "test" ->
      Linalg.init ();
      stress_test () *)
    | _ -> fail_and_usage ()
