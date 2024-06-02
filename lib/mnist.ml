(** {1 Parse MNIST CSV files} *)

(** MNIST samples *)
type t = {
  data  : int Array.t;
  label : int;
}

(** Convert one line of CSV into a MNIST sample *)
let of_line (line : string) =
  let values =
    line
    |> String.split_on_char ','
    |> List.map String.trim
    |> List.map int_of_string
  in
  { label = List.hd values; data = Array.of_list (List.tl values) }


(** Convert a CSV into a list of MNIST samples *)
let of_file ?(export=false) (fname : string) =
  let ic = open_in fname in
  let path = Printf.sprintf "%s/digit_%d.csv" (Filename.dirname fname) in
  let export_csv i l =
    let oc = open_out (path i) in
    Printf.fprintf oc "%s\n" l;
    close_out oc
  in
  ic |> In_channel.input_lines |> List.tl |> List.mapi (fun i l ->
    (if export then export_csv i l);
    of_line l
  )

let of_single_file (fname : string) =
  let ic = open_in fname in
  let r = input_line ic |> of_line in
  close_in ic; r

(** Export a sample to a PGM image *)
let export_pgm (x : t) (fname : string) =
  let oc = open_out fname in
  Printf.fprintf oc "P2\n28 28\n255\n";
  for i = 0 to 27 do
    for j = 0 to 27 do
      Printf.fprintf oc "%-3d " x.data.(28 * i + j);
    done;
    Printf.fprintf oc "\n"
  done;
  close_out oc

(** Export all samples of a CSV into PGM images *)
let export_all_pgm (fname : string) =
  let dir = Filename.dirname fname in
  List.iteri (fun i x ->
    export_pgm x (Printf.sprintf "%s/digit_%d.pgm" dir i)
  ) (of_file fname)