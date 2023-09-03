# Generated with: ./makeTestfiles A B XY 12147

#!/bin/bash

xterm -ls -e "../proxy 7655" &

read -n 1 -p "Press key when proxy has started."

xterm -ls -e "../anyReceiver X B 127.0.0.1 7655 20" &
sleep 1
xterm -ls -e "../anyReceiver Y B 127.0.0.1 7655 20" &
sleep 5

../binSender A 127.0.0.1 7655
