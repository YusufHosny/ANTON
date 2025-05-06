import socket as sock
from dto_classes import *

host = '192.168.137.1'
port = 3201

with sock.socket(sock.AF_INET, sock.SOCK_DGRAM) as s:
    s.settimeout(100)
    s.bind((host, port))

    try:
        packet, addr = s.recvfrom(5)
        if packet == b'strt\n':
            s.settimeout(.1)
            while packet != b'stop\n':
                
                packet = Packet(StepMessage(StepMessageType.RESET, 0), StepMessage(StepMessageType.RESET, 0), RacketMessage(.23, False))

                s.sendto(packet.as_bytes(), addr)

                try: packet, _ = s.recvfrom(5)
                except sock.timeout: pass
            
    except sock.timeout:
        print("Socket timeout.")

    s.sendto(b'stop\n', (host, port))