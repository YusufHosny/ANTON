import cv2 as cv
from tracking.Tracker import Tracker
from Visualizer import Visualizer
import time
import pickle

def main():
    # camera setup
    capture1 = cv.VideoCapture('tracking/video/camA.mp4')
    capture2 = cv.VideoCapture('tracking/video/camB.mp4')

    ballRange = (10, 160, 160), (24, 255, 255)

    
    state = None
    with open('./state.pkl', 'rb') as f:
        state = pickle.load(f)

    tracker = Tracker((capture1, capture2), (22, 23), (44.7, 39.6), ballRange, 'load', state)
    visualizer = Visualizer()

    tracker.start(visual=True)
    visualizer.start()

    while tracker.active:
        pt =  tracker.get_point(blocking=True)
        visualizer.queue.put(pt)


    tracker.stop()
    visualizer.stop()

if __name__ == "__main__":
    main()