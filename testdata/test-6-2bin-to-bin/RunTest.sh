# test file creation ./makeTestfiles A B X 12147
# test file creation ./makeTestfiles B B X 12148

#!/bin/bash

xterm -ls -e "../proxy 7655" &

read -n 1 -p "Press key when proxy has started."

xterm -ls -e "../anyReceiver X B 127.0.0.1 7655 20" &
sleep 5

xterm -ls -e "../binSender A 127.0.0.1 7655" &

../binSender B 127.0.0.1 7655
