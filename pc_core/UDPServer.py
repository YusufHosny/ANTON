import socket as sock
from typing import Self
import threading as ts
from struct import pack
from dto_classes import *
from queue import SimpleQueue as Queue
import time

class UDPServer:
        
    def __init__(self: Self, host: str, port: int) -> None:
        self.host = host
        self.port = port
        self.queue: Queue = Queue()
        self._done: bool = True
        self._thread: ts.Thread|None = None    

    def start(self: Self) -> None:
        self._done = False
        if self._thread is None:
            self._thread = ts.Thread(target=self._start, name='ServerThread')
            self._thread.start()
        
    def _start(self: Self) -> None:
        with sock.socket(sock.AF_INET, sock.SOCK_DGRAM) as s:
            s.settimeout(.1)
            s.bind((self.host, self.port))

            while not self._done:
                try:
                    packet, addr = s.recvfrom(5)
                    if packet == b'strt\n':
                        while packet != b'stop\n' and not self._done:
                            if not self.queue.empty():
                                packet: Packet = self.queue.get()
                                data = pack(packet.fmt(), *packet.values())
                                s.sendto(data, addr)

                            try: packet = s.recvfrom(5)[0]
                            except sock.timeout: pass

                except sock.timeout: pass
                    
            s.sendto(b'stop\n', (self.host, self.port))

    def stop(self: Self) -> None:
        self._done = True
        if self._thread is not None:
            self._thread.join()
            self._thread = None

    def isActive(self: Self) -> bool:
        return not self._done
    
    def reset(self: Self) -> None:
        if self.isActive():
            self.stop()
            self.start()


if __name__ == "__main__":
    srv = UDPServer('192.168.137.1', 3201)
    srv.start()
    for i in range(100): srv.queue.put(Packet(StepMessage(True, .1, 12), StepMessage(False, 0, 0), RacketMessage(.23, False)))
    time.sleep(20)
    srv.stop()