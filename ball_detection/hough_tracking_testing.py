import cv2 as cv
import numpy as np

camera_ip = '192.168.137.96'
camera_port = 8080

capture = cv.VideoCapture(f'http://{camera_ip}:{camera_port}/video')

dist = lambda x1, y1, x2, y2: (x1-x2)**2+(y1-y2)**2
prevCircle = None

while True:
    ret, img = capture.read()
    if not ret: break

    gray = cv.cvtColor(img, cv.COLOR_BGR2GRAY)
    blur = cv.GaussianBlur(gray, (17, 17), 0)

    circles = cv.HoughCircles(blur, cv.HOUGH_GRADIENT, 1.2, 100, 
                              param1=100, param2=30, minRadius=5, maxRadius=20)
    
    if circles is not None:
        circles = np.uint16(np.around(circles))
        chosen = None
        
        for i in circles[0, :]:
            if chosen is None: chosen = i
            if prevCircle is not None:
                if dist(chosen[0], chosen[1], prevCircle[0], prevCircle[1]) <= dist(i[0], i[1], prevCircle[0], prevCircle[1]):
                    chosen = i
        
        cv.circle(img, (chosen[0], chosen[1]), 1, (0, 100, 100), 3)
        cv.circle(img, (chosen[0], chosen[1]), chosen[2], (255, 0, 255), 3)
        prevCircle = chosen

    cv.imshow("pingpongtrack", img)
    if cv.waitKey(1) & 0xFF == ord('q'): break

capture.release()
cv.destroyAllWindows()