from dataclasses import dataclass
from typing import Tuple

@dataclass
class StepMessage:
    update: bool
    position: float
    urgency: int

    @staticmethod
    def from_data(data: Tuple[bool, float, int]):
        return StepMessage(data[0], data[1], data[2])

    @staticmethod
    def fmt(): return '?7xdB7x'

    def __repr__(self):
        return f'StepMessage(update={self.update}, position={self.position}, urgency={self.urgency})'

    def values(self):
        return self.update, self.position, self.urgency

@dataclass
class RacketMessage:
    angle: float
    fire: bool

    @staticmethod
    def from_data(data: Tuple[float, bool]):
        return RacketMessage(data[0], data[1])
    
    @staticmethod
    def fmt(): return 'd?7x'

    def __repr__(self):
        return f'RacketMessage(angle={self.angle}, fire={self.fire})'

    def values(self):
        return self.angle, self.fire
    

@dataclass
class Packet:
    hstepMsg: StepMessage
    vstepMsg: StepMessage
    rMsg: RacketMessage

    @staticmethod   
    def from_data(data: Tuple[bool, float, int, bool, float, int, float, bool]):
        return Packet(StepMessage(data[:3], data[3:6], data[6:]))

    @staticmethod
    def fmt(): return f'<{StepMessage.fmt()}{StepMessage.fmt()}{RacketMessage.fmt()}'

    def __repr__(self):
        return f'Packet(hMsg={self.hstepMsg}, vMsg={self.vstepMsg}, rMsg={self.rMsg})'

    def values(self):
        return self.hstepMsg.values() + self.vstepMsg.values() + self.rMsg.values()