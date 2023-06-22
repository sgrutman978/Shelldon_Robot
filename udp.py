import socket
import time
import sys

UDP_IP = sys.argv[1]
UDP_PORT = 8888
MESSAGE = sys.argv[2]

print("UDP target IP:", UDP_IP)
print("UDP target port:", UDP_PORT)
print("message:", MESSAGE)

sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM) # UDP
sock.sendto(bytes(f"{MESSAGE}", "utf-8"), (UDP_IP, UDP_PORT))
