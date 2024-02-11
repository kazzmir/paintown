#!/bin/sh
scalalibjar=`find ~  -name "scala-library-2*.jar" -print -quit`
classpath="build/classes/scala/main:$scalalibjar:build/resources/main"

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
