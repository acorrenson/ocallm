open Ocallm

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
    | "opencl" -> Opencl.test_opencl "hello"
    | _ -> fail_and_usage ()
