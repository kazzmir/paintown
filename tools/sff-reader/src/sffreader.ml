(*
open ExtLib
*)

(* Reads bytes in little-endian format *)
let read_bytes bytes input =
  let read byte = input_byte input in
  (* little endian, so reverse the numbers *)
  let bytes = List.rev (ExtList.List.of_enum (Enum.map read (ExtList.List.enum
  (ExtList.List.make
  bytes 0)))) in
  (* List.iter (fun byte -> Printf.printf "Got byte %d\n" byte) bytes; *)
  (* Add all the numbers up. For a 4 byte number it would be:
   *   n4 * 256^3 + n3 * 256^2 + n2 * 256^1 + n1 * 256^0
   *)
  List.fold_left (fun total now -> total * 256 + now) 0 bytes
;;

(* curried functions *)
let read_bytes_2 : in_channel -> int = read_bytes 2;;
let read_bytes_4 : in_channel -> int = read_bytes 4;;

(* Read the LBX header. See above *)
let read_header input =
  let f : int = read_bytes_2 input in
  Printf.printf "Number of files: %d\n" f;
  Printf.printf "Signature: %d\n" (read_bytes_4 input);
  Printf.printf "Version: %d\n" (read_bytes_2 input);
  for i = 0 to f do
    Printf.printf "Next file is: %x\n" (read_bytes_4 input);
  done;
  0
;;

(* Read an LBX archive. `file' is the filename *)
let read_lbx file =
  Printf.printf "Reading file %s\n" file;
  let input = open_in_bin file in
  Printf.printf "Opened file\n";
  let header = read_header input in
  close_in input;
;;

Printf.printf "Lbx reader\n";;
read_lbx Sys.argv.(1);
