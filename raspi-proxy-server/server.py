#!/usr/bin/env python
from __future__ import print_function
import logging
from lib.websocket_server import WebsocketServer
from subprocess import call
import socket

HOST = '10.0.0.2'
PORT = 10000
s1 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s1.connect((HOST, PORT))
s1.sendall(str.encode('Initializing connection'))

s2 = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s2.connect((HOST, 10001))
s2.sendall(str.encode('Initializing connection'))

def on_msg(client, server, message):
    print(message + "...", end="")
    cmd = message[0:4].lower()
    params = message[4:]
    if cmd == "say ":
        call(['ssh', 'mobility@10.0.0.2', 'echo %s >> /dev/ttyS3' % params])
        server.send_message(client, "okay")
    elif cmd == "cam ":
        coords = map(lambda x: min(1, max(-1, float(x))), params.split())
        #x = coords[0] * 875
        #y = coords[1] * 295
        s1.sendall(str.encode(
            "CAM %.2f %.2f" % (coords[0], coords[1])
        ))
        data = s1.recv(1023)
        print('Received ' + str(data))
    elif cmd == "drv ":
        coords = map(lambda x: min(1, max(-1, float(x))), params.split())
#        s2.sendall(str.encode(
#            "DRV %.2f %.2f" % (coords[0], coords[1])
#        ))
#        data = s2.recv(1023)
#        print('Received ' + str(data))


        # call(['ssh', 'mobility@10.0.0.2', './visca_cli -d /dev/ttyS2 set_pantilt_absolute_position 16 16 %i %i' % (x, y)])
        # server.send_message(client, "okay")
    else:
        server.send_message(client, "unknown command")
    print('complete!')

server = WebsocketServer(10000, host='0.0.0.0', loglevel=logging.INFO)
server.set_fn_message_received(on_msg)
server.run_forever()

