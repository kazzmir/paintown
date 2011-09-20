#!/bin/bash

if [ ! $1 ]; then
    echo "Please pass in the location of the root allegro5 source tree"
    exit
fi

basedir=$(dirname $0)
location=$(dirname $1)

# Verify location
if [ ! -d $location/include/allegro5 ]; then
    echo "Directory is not a valid location for allegro5"
    exit
fi

ln -s $basedir/SConstruct $location/
ln -s $basedir/include/allegro5/internal/* $location/include/allegro5/internal/
ln -s $basedir/include/allegro5/platform/alplatf.h $location/include/allegro5/platform/
