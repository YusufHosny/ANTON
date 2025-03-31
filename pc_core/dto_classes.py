from dataclasses import dataclass
from typing import Tuple
from enum import IntEnum
from struct import pack, unpack

class StepMessageType(IntEnum):
        NORMAL = 0
        MANUAL = 1
        RESET = 2


@dataclass
class StepMessage:
    type: StepMessageType
    position: float

    @staticmethod
    def from_data(data: Tuple[int, float]):
        return StepMessage(data[0], data[1])

    @staticmethod
    def fmt(): return 'l4xd'

    def __repr__(self):
        return f'StepMessage(type={self.type}, position={self.position})'
    
    def __len__(self):
        return 16

    def values(self):
        return int(self.type), self.position
    
    def as_bytes(self):
        return pack(self.fmt(), *self.values())
    
    @staticmethod
    def from_bytes(data):
        return StepMessage(*unpack(StepMessage.fmt(), data))
    
    @staticmethod
    def size():
        return 16
    
@dataclass
class RacketMessage:
    angle: float
    fire: bool

    @staticmethod
    def from_data(data: Tuple[float, bool]):
        return RacketMessage(data[0], data[1])
    
    @staticmethod
    def fmt(): return 'f?3x'

    def __repr__(self):
        return f'RacketMessage(angle={self.angle}, fire={self.fire})'

    def __len__(self):
        return 8

    def values(self):
        return self.angle, self.fire
    
    def as_bytes(self):
        return pack(self.fmt(), *self.values())
    
    @staticmethod
    def from_bytes(data):
        return RacketMessage(*unpack(RacketMessage.fmt(), data))
    
    @staticmethod
    def size():
        return 8

@dataclass
class Packet:
    hstepMsg: StepMessage
    vstepMsg: StepMessage
    rMsg: RacketMessage

    @staticmethod
    def fmt(): return f'<{StepMessage.fmt()}{StepMessage.fmt()}{RacketMessage.fmt()}'

    def __repr__(self):
        return f'Packet(hMsg={self.hstepMsg}, vMsg={self.vstepMsg}, rMsg={self.rMsg})'

    def __len__(self):
        return len(self.hstepMsg) + len(self.vstepMsg) + len(self.rMsg)

    def values(self):
        return self.hstepMsg.values() + self.vstepMsg.values() + self.rMsg.values()
    
    def as_bytes(self):
        return pack(self.fmt(), *self.values())
    
    @staticmethod   
    def from_bytes(data):
        data = unpack(Packet.fmt(), data)
        return Packet(StepMessage(*data[:2]), StepMessage(*data[2:4]), RacketMessage(*data[4:]))
    
    @staticmethod
    def size():
        return StepMessage.size() * 2 + RacketMessage.size()