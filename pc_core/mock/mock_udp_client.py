import socket as sock
from typing import Self
from struct import unpack
import time
from comms.dto_classes import *

host = 'localhost' # '192.168.137.1'
port = 3201

with sock.socket(sock.AF_INET, sock.SOCK_DGRAM) as s:
    s.settimeout(.2)

    try:
        s.settimeout(5)

        s.sendto(b'strt\n', (host, port))
        
        for _ in range(100):
            try:
                packet, addr = s.recvfrom(Packet.size())
                print(Packet.from_bytes(packet))
                
            except sock.timeout: print("Socket timeout. 2")

        s.sendto(b'stop\n', (host, port))
        
    except sock.timeout: print("Socket timeout.")

    s.sendto(b'stop\n', (host, port))