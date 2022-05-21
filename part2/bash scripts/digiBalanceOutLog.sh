#!/bin/bash
ip="192.168.1.128"
topic="digiBalance/output"
file="/home/gqwilson/digiBalance/outputlog.txt"
echo "Logging digiBalance/output"
echo "Subscribing to digiBalance/output"
if [ ! -f "$file" ]; then
   cat > $file
fi

cd /bin
mosquitto_sub -h $ip -p 1883 -t $topic | tee $file
