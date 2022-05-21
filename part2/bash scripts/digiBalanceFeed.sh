#!/bin/bash
ip="192.168.1.128"
topic="digiBalance/feed"
file="/home/gqwilson/digiBalance/outputlog.txt"
echo "Logging digiBalance/feed"
if [ ! -f "$file" ]; then
   cat > $file
fi

cd /bin
mosquitto_sub -h $ip -p 1883 -t $topic | tee $file
