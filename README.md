## Proxy Converter Service
Proxy service that can convert multiple Binary files to XML files and vice versa from multiple sending and receiving clients

## Compile
```bash
# From proxyService run:
$ make
```

## Run automatic test files
```bash
# From respective test folders in 'testdata' run:
$ bash ./RunTest.sh
```

## Run manual tests
```bash
# From proxyService run:
# Proxy format:
$ ./proxy portNo
# E.G:
$ ./proxy 7654

# One or more client receivers:
# Receiver format:
$ ./anyReceiver receivingFormat receivingID IP portNo waitingTimeSec
# E.G:
$ ./anyReceiver X X 127.0.0.1 7654 30

# One or more client senders:
# Sender format:
$ ./xml(or bin)Sender senderID IP portNo' or './binSender senderID IP portNo
# E.G:
$ ./xmlSender A 127.0.0.1 7654
```

## Program description
```bash
The program is a implementation of a proxy service written in C.
Main functionality is to establish multiple connections from sending and receiving clients.
After connections are established, the proxy receives one or more byte streams from one or more sending clients in a form of .xml or .bin format.
The proxy then converts the .xml or .bin format to whatever is expected from the receiving clients.
The proxy can have connections to multiple sending and receiving clients on each side.

The 'testdata' file contains folders with testdata files that have a given input file (.xml/.bin), 
a (.xml/.bin) file with expected output data to compare with the written file from the receiving client, 
and a RunTest.sh script that will run the files accordingly.

All the tests will involve the proxy, one or more anyReceiver's and one or more sending clients (xmlSender or binSender)
Every RunTest.sh script starts the proxy first in one terminal, 
then one or more anyReceivers in their own terminals as their own receiving client, 
and lastly one or more xmlSenders in their own terminals as their own sending clients.

The proxy has the format './proxy portNo'
The receivers have the format './anyReceiver receivingFormat receivingID IP portNo waitingTimeSec'
The senders have the format './xmlSender senderID IP portNo' or './binSender senderID IP portNo'
```