import socket
import time
import select

connections = []

for i in range(5, 800):
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect_ex(('127.0.0.1', 12345))
    connections.append(s)

time.sleep(20)
