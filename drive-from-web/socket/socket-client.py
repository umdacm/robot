import socket

HOST = 'localhost'
PORT = 8080
s =  socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.connect((HOST, PORT))
try:
    while True:
        s.sendall(str.encode(input()))
        data = s.recv(1023)
        print('Received ' + str(data))
except KeyboardInterrupt:
    pass
