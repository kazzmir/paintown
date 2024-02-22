#!/bin/bash

# Install xvfb 
sudo apt install -y xvfb

# Setup xvfb for screen 0
Xvfb :1 -screen 0 800x600x24 &

# Test
timeout 3s ./paintown --data `pwd`/data | tee paintown.log 

# Check if the application started and terminated successfully based on the log
if grep -q "Set graphics mode: Ok" paintown.log && grep -q "Bye" paintown.log ; then
    echo "Test passed"
else
    echo "Test failed"
    exit 1
fi
