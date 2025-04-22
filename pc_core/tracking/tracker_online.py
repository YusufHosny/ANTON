import cv2 as cv
import numpy as np
from Tracker import Tracker
from utils import *

def main():
    # camera setup
    camera_ip1 = '192.168.0.162'
    camera_ip2 = '192.168.0.204'
    camera_port = 8080
    capture1 = cv.VideoCapture(f'http://{camera_ip1}:{camera_port}/video')
    capture2 = cv.VideoCapture(f'http://{camera_ip2}:{camera_port}/video')

    ballRange = (8, 125, 160), (24, 255, 255)

    tracker = Tracker((capture1, capture2), (22, 23), (44.7, 39.6), ballRange, 'auto')

    tracker.start(visual=True)

    while tracker.active:
        print(tracker.get_point(blocking=True))

    tracker.stop()

if __name__ == "__main__":
    main()