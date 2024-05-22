(** OpenCL bindings *)
open Linalg

external vec_add : Vector.t -> Vector.t -> Vector.t -> unit = "vec_add_stub"

external vec_dot : Vector.t -> Vector.t -> float = "vec_dot_stub"