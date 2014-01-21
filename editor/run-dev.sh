#!/bin/sh

classpath="target/scala-2.10/classes:lib/scala-library-2.10.3.jar:resources"

case "$1" in
 animator)
         shift
         java -classpath $classpath com.rafkind.paintown.animator.Animator2 $*
         ;;
 
 platformer)
         shift
         java -classpath $classpath com.rafkind.platformer.MapEditor $*
         ;;
 "")
         echo "To run the character animator: ./run.sh animator"
         java -classpath $classpath -client com.rafkind.paintown.level.Editor2 $*
         ;;
esac
