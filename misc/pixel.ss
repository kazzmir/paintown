#lang scheme
;; Show the pixel value and rgb values that loadpng thinks a sprite has

(require (planet "util.ss" ("kazzmir" "allegro.plt")))
(require (prefix-in image- (planet "image.ss" ("kazzmir" "allegro.plt"))))

(define (show picture x y)
  (easy-init 1 1 16)
  (let ((image (image-create-from-file picture)))
    (let ((pixel (image-getpixel image x y)))
      (let-values (((r g b) (image-get-rgb pixel)))
	(printf "~a,~a: ~a. RGB ~a ~a ~a\n" x y pixel r g b)))
    (image-destroy image))
  (easy-exit))

(if (< (vector-length (current-command-line-arguments)) 3)
  (begin
    (printf "pixel.ss pic.png x y\n"))
  (let ((pic (vector-ref (current-command-line-arguments) 0))
	(x (string->number (vector-ref (current-command-line-arguments) 1)))
	(y (string->number (vector-ref (current-command-line-arguments) 2))))
    (show pic x y)))
