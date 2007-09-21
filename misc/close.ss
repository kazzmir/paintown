;; Use this script to convert an image that has pixel values close
;; to pixels in a different image within some hard coded range

(require (planet "util.ss" ("kazzmir" "allegro.plt")))
(require (prefix image- (planet "image.ss" ("kazzmir" "allegro.plt"))))
(require (lib "list.ss"))

;; return the absolute difference between two pixel values
;; r^2 + g^2 + b^2
(define (difference pixel1 pixel2)
  (define (sqr x)
    (* x x))
  (let-values (((r1 g1 b1) (image-get-rgb pixel1))
	       ((r2 g2 b2) (image-get-rgb pixel2)))
    (+ (sqr (- r2 r1))
       (sqr (- g2 g1))
       (sqr (- b2 b1)))))

(define (find-best hash color)
  (define (keys)
    (hash-table-map hash (lambda (k v) k)))
  (let ((best (foldl
		(lambda (pixel so-far)
		  (cond
		    ((not so-far) pixel)
		    ((< (difference pixel color)
			(difference so-far color))
		     pixel)
		    (else so-far)))
		#f
		(keys))))
    #;(printf "Best ~s for ~a is ~a difference = ~a\n" (keys) color best (difference best color))
    (if (< (difference best color) 2500)
      best
      color)))

(define (for-each-pixel image func)
  (let yloop ((y 0))
    (when (< y (image-height image))
      (let xloop ((x 0))
	(when (< x (image-width image))
	  (func x y)
	  (xloop (add1 x))))
      (yloop (add1 y)))))

(define (read-colors image)
  (let ((hash (make-hash-table)))
    (for-each-pixel
      image
      (lambda (x y)
	(hash-table-put! hash
			 (image-getpixel image x y)
			 0)))
    hash))

(define (convert original guess out)
  (define (check image name)
    (when (not image)
      (error 'convert (format "Could not load ~a" name))))
  (easy-init 1 1 16)
  (let ((original-pic (image-create-from-file original))
	(guess-pic (image-create-from-file guess)))
    (with-handlers ((exn? (lambda (e)
			    (easy-exit)
			    (raise e))))
		   (check original-pic original)
		   (check guess-pic guess))
    (let ((colors (read-colors original-pic))
	  (new-image (image-create (image-width guess-pic)
				   (image-height guess-pic))))
      (for-each-pixel
	guess-pic
	(lambda (x y)
	  (let ((pixel (image-getpixel guess-pic x y)))
	    (image-putpixel new-image x y
			    (let ((n (find-best colors pixel)))
			      #;
			      (when (not (= n pixel))
				(printf "Replace (~a,~a) ~a with ~a original ~a\n" x y pixel n (image-getpixel original-pic x y)))
			      n)))))
      (image-save new-image out)
      (image-destroy new-image))
    (image-destroy original-pic)
    (image-destroy guess-pic)
    (easy-exit)))

(if (< 3 (vector-length (current-command-line-arguments)))
  (begin
    (printf "close.ss original.png guess.png out.png"))
  (let ((original (vector-ref (current-command-line-arguments) 0))
	(guess (vector-ref (current-command-line-arguments) 1))
	(out (vector-ref (current-command-line-arguments) 2)))
    (convert original guess out)))
