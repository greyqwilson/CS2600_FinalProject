#!/bin/bash
ip="192.168.1.128"
topic="digiBalance/setMode"
file="/home/gqwilson/digiBalance/modelog.txt"
echo "Logging digiBalance/setMode"
if [ ! -f "$file" ]; then
   cat > $file
fi

cd /bin
mosquitto_sub -h $ip -p 1883 -t $topic | tee $file

