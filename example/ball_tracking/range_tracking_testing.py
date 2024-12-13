import cv2 as cv
import numpy as np
from collections import deque

# camera setup
camera_ip = '192.168.137.96'
camera_port = 8080
capture = cv.VideoCapture(f'http://{camera_ip}:{camera_port}/video')

ballRange = ( (0, 125, 125), (25, 255, 255) ) # tuple with the range of hsv values that are considered a ball (orange range)
positions = deque(maxlen=100) # store 100 past positions of ball

while True:
    ret, img = capture.read()
    if not ret: break

    blur = cv.GaussianBlur(img, (15, 15), 0) # blue the image
    hsv = cv.cvtColor(blur, cv.COLOR_BGR2HSV) # convert RGB data to HSV
    
    # get areas of picture in hsv ball range
    mask = cv.inRange(hsv, ballRange[0], ballRange[1])

    # erode and dilate smaller areas that arent the ball
    mask = cv.erode(mask, None, iterations=2)
    mask = cv.dilate(mask, None, iterations=2)

    # get center of ball
    cnts, _ = cv.findContours(mask.copy(), cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)
    center = None
    if len(cnts) > 0:
        c = max(cnts, key=cv.contourArea)
        ((x, y), rad) = cv.minEnclosingCircle(c)
        M = cv.moments(c)
        center = (int(M['m10']/M['m00']), int(M['m01']/M['m00']))

        if rad > 10: # only show circles big enough
            cv.circle(img, (int(x), int(y)), int(rad), (0, 255, 255), 2)
            cv.circle(img, center, 5, (0, 255, 255), -1)

    cv.imshow("pingpongtrack", img)
    if cv.waitKey(1) & 0xFF == ord('q'): break

capture.release()
cv.destroyAllWindows()