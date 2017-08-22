#!/bin/bash

# Usage:
# recorder.sh <task>
if [[ ! $1 ]]
  then
    echo 'Usage: recorder.sh <task>'
    exit
fi


# Config
port=/dev/ttyACM0
baud=9600
destdir=$HOME/.task-timer
destfile=$destdir/`date +%d-%m-%y`

if [ ! -d $destdir ]; then
  mkdir $destdir
fi


# Serial set up
stty -F $port $baud raw

# Background read
cat $port >> $destfile &
bgPid=$!

# Send task to arduino
sleep 0.5
echo $1 > $port

# Kill process when finish
sleep 2 
kill $bgPid
