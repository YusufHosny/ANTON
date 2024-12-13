import socket as sock
from typing import Self

host = '192.168.137.1'
port = 3201

with sock.socket(sock.AF_INET, sock.SOCK_DGRAM) as s:
    s.settimeout(100)
    s.bind((host, port))
    cnt = 0
    try:
        packet, addr = s.recvfrom(5)
        print(packet)
        if packet == b'strt\n':
            s.settimeout(.1)
            while packet != b'stop\n':
                cnt = (cnt + 1) % 100
                s.sendto(bytearray(str(cnt), 'utf-8'), addr)
                try: packet, addr = s.recvfrom(5)
                except sock.timeout: pass
            
    except sock.timeout:
        print("Socket timeout.")

    s.sendto(b'stop\n', (host, port))