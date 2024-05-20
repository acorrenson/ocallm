(** {1 Wikitext Sanitizer} *)

(** Wrapper arround [Bytes] for lexing *)
module Lexbuf = struct
  type t = {
    buffer : Bytes.t;
    mutable position : int;
  }

  let make (s : string) : t = {
    buffer = Bytes.of_string s;
    position = 0;
  }
  
  let peek (lex : t) : char option =
    if lex.position < Bytes.length lex.buffer then
      Some (Bytes.get lex.buffer lex.position)
    else
      None

  let next_char (lex : t) : char option =
    match peek lex with
    | None -> None
    | Some c -> lex.position <- lex.position + 1; Some c

  let is_white (lex : t) : bool option =
    match peek lex with
    | None -> None
    | Some c -> Some (c <= ' ')

  let rec skip_white (lex : t) : unit option =
    match is_white lex with
    | None -> None
    | Some true ->
      lex.position <- lex.position + 1;
      skip_white lex
    | Some false -> Some ()

  let is_empty (lex : t) : bool =
    lex.position >= Bytes.length lex.buffer

  let try_match (lex : t) (x : string) : bool =
    let len = String.length x in
    if lex.position + len > Bytes.length lex.buffer then
      false
    else if x <> Bytes.sub_string lex.buffer lex.position len then
      false
    else
      (lex.position <- lex.position + len; true)

end

(** Sanitizer for Wikitext. Extract pure english from markup. *)
module Sanitizer = struct

  let (let*) = Option.bind

  type t = {
    input : in_channel;
    output : out_channel;
    mutable lex : Lexbuf.t;
  }

  let make (src_file : string) (dst_file : string) =
    let input = open_in src_file in
    let output = open_out dst_file in
    let line = input_line input in
    let lex = Lexbuf.make line in
    { input; output; lex }

  let rec new_line (p : t) : string option =
    try
      let line = input_line p.input in
      if line = "" then new_line p
      else Some line
    with End_of_file -> None

  let refill (p : t) : bool option =
    if Lexbuf.is_empty p.lex then begin
      let* line = new_line p in
      p.lex <- Lexbuf.make line;
      Some true
    end else
      Some false

  let skip_line (p : t) : unit option =
    Printf.fprintf p.output "\n";
    let* line = new_line p in
    Some (p.lex <- Lexbuf.make line)

  let print_current_position (p : t) =
    let buf = p.lex.buffer in
    let pos = p.lex.position in
    let len = min (pos + 10) (Bytes.length buf - pos) in
    let next = Bytes.sub buf pos len |> Bytes.to_string in
    Printf.printf "currently at \"%s\"...\n" next


  let rec skip_white (p : t) : unit option =
    match Lexbuf.skip_white p.lex with
    | None ->
      let* r = refill p in
      if r then skip_white p else Some ()
    | _ -> Some ()

  let rec skip_char (p : t) =
    match Lexbuf.next_char p.lex with
    | None -> let* _ = refill p in skip_char p
    | Some _ -> Some ()

  let rec next_char (p : t) : char option =
    match Lexbuf.next_char p.lex with
    | None -> let* _ = refill p in next_char p
    | res -> res

  let close_brackets (p : t) (b_open : string) (b_close : string) : unit option =
    let rec go count =
      if Lexbuf.try_match p.lex b_close then
        if count <= 0 then Some ()
        else go (count - 1)
      else if Lexbuf.try_match p.lex b_open then
        go (1 + count)
      else let* () = skip_char p in go count
    in go 0

  let close_quoted (p : t) : string option =
    let rec go str =
      if Lexbuf.try_match p.lex "'''" then
        Some str
      else let* c = next_char p in go (str ^ String.make 1 c)
    in go ""

  
  let rec next_word (p : t) : string option =
    let* () = skip_white p in
    if Lexbuf.try_match p.lex "[[File:" then
      let* () = close_brackets p "[[" "]]" in
      next_word p
    else if Lexbuf.try_match p.lex "[[Image:" then
      let* () = close_brackets p "[[" "]]" in
      next_word p
    else if Lexbuf.try_match p.lex "{{Infobox" then
      let* () = close_brackets p "{{" "}}" in
      next_word p
    else if Lexbuf.try_match p.lex "=" then
      let* () = skip_line p in next_word p
    else if Lexbuf.try_match p.lex "==" then
      let* () = skip_line p in next_word p
    else if Lexbuf.try_match p.lex "===" then
      let* () = skip_line p in next_word p
    else if Lexbuf.try_match p.lex "====" then
      let* () = skip_line p in next_word p
    else if Lexbuf.try_match p.lex "'''" then
      close_quoted p
    else None

  let rec run (p : t) : unit =
    match next_word p with
    | None -> print_current_position p
    | Some w ->
      Printf.fprintf p.output "%s " w;
      run p

  let sanitize (f : string) : unit =
    let p = make f (f ^ ".sanitized") in
    run p;
    close_in p.input;
    close_out p.output
end


