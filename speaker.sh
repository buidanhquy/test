#!/bin/bash

dev1=$1
dev2=$2
dev3=$3
dev4=$4

if [ -n "$dev1" ]; then
    echo "not null: $dev1"
    aplay -D hw:$dev1,0 vnt.wav
else
    echo "null"
fi
 
if [ -n "$dev2" ]; then
    echo "not null: $dev2"
    aplay -D hw:$dev2,0 vnt.wav
else
    echo "null"
fi

if [ -n "$dev3" ]; then
    echo "not null: $dev3"
else
    echo "null"
fi

if [ -n "$dev4" ]; then
    echo "not null: $dev4"
else
    echo "null"
fi

