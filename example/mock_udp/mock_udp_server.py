import socket as sock
from typing import Self
from struct import pack
import time
from dto_classes import *

host = '192.168.137.1'
port = 3201


with sock.socket(sock.AF_INET, sock.SOCK_DGRAM) as s:
    s.settimeout(100)
    s.bind((host, port))
    
    update, position, urgency = True, .9, 10
    try:
        packet, addr = s.recvfrom(5)
        if packet == b'strt\n':
            s.settimeout(.1)
            while packet != b'stop\n':
                
                packet = Packet(                
                    StepMessage(True, .9, 10),
                    StepMessage(True, .1, 40),
                    RacketMessage(.2, True)
                )
                
                data = pack(
                    packet.fmt(), 
                    *packet.values()
                )

                s.sendto(data, addr)

                try: packet, _ = s.recvfrom(5)
                except sock.timeout: pass
            
    except sock.timeout:
        print("Socket timeout.")

    s.sendto(b'stop\n', (host, port))