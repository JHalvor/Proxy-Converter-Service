# Generated with: ./makeTestfiles A X XY 12147

#!/bin/bash

xterm -ls -e "../proxy 7655" &

read -n 1 -p "Press key when proxy has started."

xterm -ls -e "../anyReceiver X X 127.0.0.1 7655 20" &
sleep 1
xterm -ls -e "../anyReceiver Y X 127.0.0.1 7655 20" &
sleep 5

../xmlSender A 127.0.0.1 7655
