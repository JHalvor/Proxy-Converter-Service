# Testfile generator: ./makeTestfiles A X X 11133

#!/bin/bash

xterm -ls -e "../proxy 7654" &

read -n 1 -p "Press key when proxy has started."

xterm -ls -e "../anyReceiver X X 127.0.0.1 7654 30" &
sleep 5

../xmlSender A 127.0.0.1 7654
