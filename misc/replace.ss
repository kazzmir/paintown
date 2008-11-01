#lang scheme

;; Show the pixel value and rgb values that loadpng thinks a sprite has

(require (planet "util.ss" ("kazzmir" "allegro.plt")))
(require (prefix-in image- (planet "image.ss" ("kazzmir" "allegro.plt"))))

(define (for-each-pixel image func)
  (let yloop ((y 0))
    (when (< y (image-height image))
      (let xloop ((x 0))
	(when (< x (image-width image))
	  (func x y)
	  (xloop (add1 x))))
      (yloop (add1 y)))))

(define (convert-val v)
  (let ((num (regexp-match (regexp "num=([0-9]+)") v))
	(rgb (regexp-match (regexp "rgb=([0-9]+),([0-9]+),([0-9]+)") v)))
    (cond
      (num (string->number (list-ref num 1)))
      (rgb (image-color (string->number (list-ref rgb 1))
			(string->number (list-ref rgb 2))
			(string->number (list-ref rgb 3))))
      (else (error 'convert-value "Invalid format '~a'" v)))))

(define (replace picture val1 val2)
  (printf "Replacing ~a with ~a\n" val1 val2)
  (let ((image (image-create-from-file picture)))
    (for-each-pixel image
		    (lambda (x y)
		      (let ((p (image-getpixel image x y)))
			(when (= p val1)
			  (image-putpixel image x y val2)))))
    (image-save image picture)
    (image-destroy image)))

(if (< (vector-length (current-command-line-arguments)) 3)
  (begin
    (printf "replace pic.png val1 val2\n")
    (printf "val: num=# or rgb=#,#,#\n"))
  (begin
    (easy-init 1 1 16)
    (let ((pic (vector-ref (current-command-line-arguments) 0))
	  (val1 (convert-val (vector-ref (current-command-line-arguments) 1)))
	  (val2 (convert-val (vector-ref (current-command-line-arguments) 2))))
	(replace pic val1 val2))
    (easy-exit)))
