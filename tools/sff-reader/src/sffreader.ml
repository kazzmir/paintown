(* Sffreader for mugen .sff files *)

(*
open ExtLib
*)

let read_bytes bytes input =
  let read byte = input_byte input in
  let bytes = List.rev (ExtList.List.of_enum (Enum.map read (ExtList.List.enum
  (ExtList.List.make
  bytes 0)))) in
  (* List.iter (fun byte -> Printf.printf "Got byte %d\n" byte) bytes; *)
  (* Add all the numbers up. For a 4 byte number it would be:
   *   n4 * 256^3 + n3 * 256^2 + n2 * 256^1 + n1 * 256^0
   *)
  let answer = List.fold_left (fun total now -> total * 256 + now) 0 bytes in
  (*
  List.iter (fun y -> Printf.printf "%d, " y) bytes;
  Printf.printf " = %u\n" answer;
  *)
  answer
;;

(* curried functions *)
let read_bytes_1 : in_channel -> int = read_bytes 1;;
let read_bytes_2 : in_channel -> int = read_bytes 2;;
let read_bytes_4 : in_channel -> int = read_bytes 4;;

let read_sprite input =
  let here = pos_in input in 
  let nextLocation = read_bytes_4 input in
  let length = read_bytes_4 input in
  let x = read_bytes_2 input in
  let y = read_bytes_2 input in
  let groupNumber = read_bytes_4 input in
  let imageNumber = read_bytes_4 input in
  let previous = read_bytes_4 input in
  let samePalette = read_bytes_1 input in
  (*
  let comments = read_bytes 13 input in
  *)
  let realLength = nextLocation - here - 32 in
  Printf.printf "  Length %d x %d y %d\n" realLength x y;
  nextLocation
;;

let read_header input =
  (* skip first 16 bytes *)
  seek_in input 16;
  let totalGroups = read_bytes_4 input in
  let totalImages = read_bytes_4 input in
  let suboffset = read_bytes_4 input in
  let subhead = read_bytes_4 input in
  let sharedPalette = read_bytes_1 input in
  Printf.printf "Total groups %u\n" totalGroups;
  Printf.printf "Total images %u\n" totalImages;
  seek_in input suboffset;

  for i = 0 to totalImages do
    Printf.printf "Image %d\n" i;
    seek_in input (read_sprite input)
  done;

  0
;;

let read_sff file =
  Printf.printf "Reading file %s\n" file;
  let input = open_in_bin file in
  Printf.printf "Opened file\n";
  let header = read_header input in
  close_in input;
;;

if Array.length Sys.argv < 2 then
  Printf.printf "Give a .sff file as an argument\n"
else begin
  read_sff Sys.argv.(1);
end
