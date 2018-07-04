#!/user/bin/env python
from __future__ import print_function
import logging
from websocket_server import WebsocketServer
from subprocess import call
import socket

import socket

HOST = '10.0.0.2'
PORT = 10000
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))

def on_msg(client, server, message):
    print(message + "...", end="")
    cmd = message[0:4].lower()
    params = message[4:]
    if cmd == "say ":
        call(['ssh', 'mobility@10.0.0.2', 'echo %s >> /dev/ttyS3' % params])
        server.send_message(client, "okay")
    elif cmd == "cam ":
        coords = map(lambda x: min(1, max(-1, float(x))), params.split())
        x = coords[0] * 875
        y = coords[1] * 295
        s.sendall(str.encode(
            "CAM %.2f %.2f" % coords
        ))
        data = s.recv(1023)
        print('Received ' + str(data))

        # call(['ssh', 'mobility@10.0.0.2', './visca_cli -d /dev/ttyS2 set_pantilt_absolute_position 16 16 %i %i' % (x, y)])
        # server.send_message(client, "okay")
    else:
        server.send_message(client, "unknown command")
    print('complete!')

server = WebsocketServer(10000, host='0.0.0.0', loglevel=logging.INFO)
server.set_fn_message_received(on_msg)
server.run_forever()
