#!/bin/bash

for f in `ls ./`;do
	if [ -d "$f" ]; then
		# bleh not directories
		echo "skipping directory"
	else
		convert -fill "rgb(255,0,255)" -opaque transparent -draw "color 0,0 floodfill" -trim $f new-$f 1>x
		echo "Converted $f to new-$f"
	fi
done

