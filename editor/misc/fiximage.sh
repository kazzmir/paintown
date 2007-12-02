#!/bin/bash

# I guess imagemagick can support other formats... if you need one add it to the list

extra=""
replace="no"
while getopts fr o
do
	case $o in
		f) extra="$extra -flop";;
		r) replace="yes";;
	esac
done

for f in $(find ./ -iname "*.png" -or -iname "*.bmp" -or -iname "*.jpg" -or -iname "*.jpeg" -or -iname "*.gif" | sed -e 's/\.\///g');do
	if [ $replace = "yes" ]; then
		convert -fill "rgb(255,0,255)" -opaque transparent -draw "color 0,0 floodfill" $extra -trim $f $f
		echo "Converted $f"
	else
		convert -fill "rgb(255,0,255)" -opaque transparent -draw "color 0,0 floodfill" $extra -trim $f new-$f
		echo "Converted $f to new-$f"
	fi
done

