# To build this document you need the 'scribble' tool from the racket system
# http://racket-lang.org

files=main.scrbl # whatever.scrbl
main.pdf: *.scrbl
	raco scribble --pdf main.scrbl

.PHONY: html

html: *.scrbl
	raco scribble --htmls main.scrbl
