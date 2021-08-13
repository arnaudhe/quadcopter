import socket
from datetime import datetime

UDP_IP   = "0.0.0.0"
UDP_PORT = 5002

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
sock.bind((UDP_IP, UDP_PORT))

while True:
    data = sock.recv(128)
    print(f'{datetime.now()} {data.decode("utf-8")}')