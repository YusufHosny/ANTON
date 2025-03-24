import socket as sock
from typing import Self
from struct import unpack
import time
from dto_classes import *

host = 'localhost' # '192.168.137.1'
port = 3201

with sock.socket(sock.AF_INET, sock.SOCK_DGRAM) as s:
    s.settimeout(.2)

    try:
        s.settimeout(5)

        s.sendto(b'strt\n', (host, port))
        
        for _ in range(3):
            try:
                packet, addr = s.recvfrom(24+24+16)
                result = unpack(f'<{StepMessage.fmt}{StepMessage.fmt}{RacketMessage.fmt}', packet)
                print(Packet.from_data(result))
                
            except sock.timeout:
                print("Socket timeout.")

        s.sendto(b'stop\n', (host, port))
        
    except sock.timeout:
        print("Socket timeout.")

    s.sendto(b'stop\n', (host, port))