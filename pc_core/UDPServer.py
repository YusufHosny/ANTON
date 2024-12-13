import socket as sock
from typing import Self

class MockUDPServer:
        
    def __init__(self: Self, host: str, port: int):
         self.host = host
         self.port = port
    
    def start(self: Self):
        with sock.socket(sock.AF_INET, sock.SOCK_DGRAM) as s:
            s.settimeout(5)

            s.sendto(b'strt\n', (self.host, self.port))
            while not self._done:
                try:
                    packet, addr = s.recvfrom(10)

                except sock.timeout:
                    print("Socket timeout, no data received.")

            s.sendto(b'stop\n', (self.host, self.port))