#!/bin/sh

case "$1" in
     animator)
	java -classpath editor.jar com.rafkind.paintown.animator.Animator
	;;
     "")
	java -jar editor.jar -client
	;;
esac
