import socket
s = socket.socket()
s.connect(('localhost', 8080))
# bad req
s.sendall(b'HELLO /index.html\n\n')

response = s.recv(4096)
print(response.decode())

s.close()
