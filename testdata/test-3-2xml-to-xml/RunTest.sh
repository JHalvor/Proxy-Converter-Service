# test file creation ./makeTestfiles A X X 12147
# test file creation ./makeTestfiles B X X 12148

#!/bin/bash

xterm -ls -e "../proxy 7655" &

read -n 1 -p "Press key when proxy has started."

xterm -ls -e "../anyReceiver X X 127.0.0.1 7655 20" &
sleep 5

xterm -ls -e "../xmlSender A 127.0.0.1 7655" &

../xmlSender B 127.0.0.1 7655
