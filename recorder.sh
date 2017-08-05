#!/bin/bash

#TODO 
#insert timestamp

# Usage:
# timer.sh <task>

if [[ ! $1 ]]
  then
    echo 'Usage: timer.sh <task>'
    exit
fi

#config
port=/dev/ttyACM0
baud=9600
dest=tasks.dat

#serial set up
stty -F $port $baud raw

#background read
cat $port >> $dest &
bgPid=$!

#send task to arduino
sleep 0.5
echo $1 > $port

#kill process when finish
sleep 2 
kill $bgPid
