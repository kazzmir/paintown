#!/bin/sh

case "$1" in
 animator)
         shift
         java -classpath editor.jar com.rafkind.paintown.animator.Animator2 $*
         ;;
 "")
         echo "To run the character animator: ./run.sh animator"
         java -jar editor.jar -client
         ;;
esac
