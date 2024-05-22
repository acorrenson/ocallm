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

let () =
  if Array.length Sys.argv < 2 then
    fail_and_usage ()
  else
    match Sys.argv.(1) with
    | "sanitize" -> sanitize ()
    | "opencl" ->
      let c = Bigarray.(Array1.of_array int32 c_layout) [| 0l; 0l; 0l; 0l; |] in
      let a = Bigarray.(Array1.of_array int32 c_layout) [| 1l; 2l; 3l; 4l; |] in
      let b = Bigarray.(Array1.of_array int32 c_layout) [| 3l; 4l; 5l; 6l; |] in
      Opencl.vec_addi a b c;
      let c = Linalg.Vector.of_array [| 0.; 0.; 0.; 0.; |] in
      let a = Linalg.Vector.of_array [| 1.; 2.; 3.; 4.; |] in
      let b = Linalg.Vector.of_array [| 3.; 4.; 5.; 6.; |] in
      Opencl.vec_add a b c;
      for i = 0 to 3 do
        Printf.printf "%f +. %f = %f\n" a.{i} b.{i} c.{i}
      done
    | _ -> fail_and_usage ()
