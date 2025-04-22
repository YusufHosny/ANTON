import cv2 as cv
import numpy as np
from Tracker import Tracker
from utils import *

def main():
    # camera setup
    capture1 = cv.VideoCapture('video/camA.mp4')
    capture2 = cv.VideoCapture('video/camB.mp4')

    ballRange = (10, 160, 160), (24, 255, 255)

    tracker = Tracker((capture1, capture2), (22, 23), (44.7, 39.6), ballRange, 'auto', 4)

    tracker.start(visual=True)

    while tracker.active:
        print(tracker.get_point(blocking=True))

    tracker.stop()

if __name__ == "__main__":
    main()