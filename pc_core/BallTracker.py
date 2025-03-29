import cv2 as cv
import numpy as np


def closest_point_between_skew_lines(p1, d1, p2, d2):
    """
    Finds the closest point between two skew lines in 3D space.
    
    Parameters:
        p1 (np.array): A point on the first line (3D vector)
        d1 (np.array): Direction vector of the first line (3D vector)
        p2 (np.array): A point on the second line (3D vector)
        d2 (np.array): Direction vector of the second line (3D vector)
    
    Returns:
        (np.array): The closest point to both lines
    """
    p1, d1, p2, d2 = map(np.asarray, (p1, d1, p2, d2))
    
    # Compute the normal to both direction vectors
    n = np.cross(d1, d2)
    n_norm = np.linalg.norm(n)
    
    if n_norm == 0:
        raise ValueError("The lines are parallel or coincident, no unique closest points.")
    
    n = n / n_norm  # Normalize normal
    
    # Compute the matrix components
    d1d1 = np.dot(d1, d1)
    d1d2 = np.dot(d1, d2)
    d2d2 = np.dot(d2, d2)
    
    # Compute the right-hand side
    rhs = p2 - p1
    rhs_d1 = np.dot(rhs, d1)
    rhs_d2 = np.dot(rhs, d2)
    
    # Solve for t and s (parameters for closest points)
    denom = d1d1 * d2d2 - d1d2 ** 2
    if abs(denom) < 1e-10:
        raise ValueError("The lines are nearly parallel, numerical instability may occur.")
    
    t = (rhs_d1 * d2d2 - rhs_d2 * d1d2) / denom
    s = (rhs_d1 * d1d2 - rhs_d2 * d1d1) / denom
    
    # Compute closest points
    closest_p1 = p1 + t * d1
    closest_p2 = p2 + s * d2
    
    return (closest_p1 + closest_p2)/2

def image_to_world_plane(u, v, H):
    point = np.array([u, v, 1])
    projected = H @ point  # Matrix multiplication
    projected = projected / projected[2]  # Normalize
    return projected[0], projected[1], 0.0  # (X, Y, 0)


def calibrate_camera(img, focal_length, sensor_width, flip_coordinates):

    bottom = 0
    top = 0
    left_inner = 0
    right_inner = 0

    def gen_mask(image, lower, upper, left, right):
        (h, w) = image.shape[:2]
        mask = np.zeros((h,w),np.uint8)
        cv.rectangle(mask,(0,int(lower)),(w, int(upper)), 255, -1)
        cv.rectangle(mask,(int(left), int(lower + 25)),(int(right), int(upper - 40)), 0, -1)
        return mask

    def set_mask(event, x, y, flags, param):
        nonlocal bottom, top, left_inner, right_inner
        if event == cv.EVENT_LBUTTONUP:
            if top == 0 or bottom == 0:
                if top == 0:
                    top = y
                elif y < top:
                    bottom = top
                    top = y
                else:
                    bottom = y
            elif left_inner == 0:
                left_inner = x
            elif x < left_inner:
                right_inner = left_inner
                left_inner = x
            else:
                right_inner = x
    

    cv.namedWindow("Original")
    cv.setMouseCallback("Original", set_mask)
    cv.imshow("Original", img)
    while True:
        cv.waitKey(500)
        if top != 0 and bottom != 0 and left_inner != 0 and right_inner != 0:
            break
    
    mask = gen_mask(img, top  , bottom, left_inner, right_inner)
    masked_img = cv.bitwise_and(img, img, mask=mask)
    cv.imshow("masked image", masked_img)
    cv.waitKey(0)
    cv.destroyWindow("masked image")

    hsv = cv.cvtColor(masked_img, cv.COLOR_BGR2HSV)

    lower_bound = np.array([0,0,200])
    upper_bound = np.array([179,110,255])

    mask = cv.inRange(hsv, lower_bound, upper_bound)
    cdst = cv.cvtColor(mask, cv.COLOR_GRAY2BGR)
    Plines = cv.HoughLinesP(mask, 1, np.pi / 180, 100, None, 150, 50)

    BottomLines = [[],[]]
    TopLines = [[],[]]
    LeftLines = [[],[]]
    RightLines = [[],[]]


    if Plines is not None:
        for i in range(0, len(Plines)):
            l = Plines[i][0]
            rico = ((l[3]-l[1])/(l[2]-l[0]))
            intersect = l[1] - rico * l[0]
            print(rico)
            if -0.4 < rico < 0.4:
                if top - 50 < l[1] < top + 50:
                    TopLines[0].append(rico)
                    TopLines[1].append(intersect)
                else:
                    BottomLines[0].append(rico)
                    BottomLines[1].append(intersect)
            else:
                if rico < 0:
                    LeftLines[0].append(rico)
                    LeftLines[1].append(intersect)
                else:
                    RightLines[0].append(rico)
                    RightLines[1].append(intersect)

            cv.line(cdst, (l[0], l[1]), (l[2], l[3]), (0,0,255), 3, cv.LINE_AA)
        cv.namedWindow('Detected Lines (in red) - Probabilistic Line Transform')
        cv.imshow("Detected Lines (in red) - Probabilistic Line Transform", cdst)
        cv.waitKey(0)
        cv.destroyWindow("Detected Lines (in red) - Probabilistic Line Transform")
    
    TopLine = (sum(TopLines[0])/len(TopLines[0]), sum(TopLines[1])/len(TopLines[1]))
    BottomLine = (sum(BottomLines[0])/len(BottomLines[0]), sum(BottomLines[1])/len(BottomLines[1]))
    LeftLine = (sum(LeftLines[0])/len(LeftLines[0]), sum(LeftLines[1])/len(LeftLines[1]))
    RightLine = (sum(RightLines[0])/len(RightLines[0]), sum(RightLines[1])/len(RightLines[1]))


    TopLeft = ((LeftLine[1] - TopLine[1]) / (TopLine[0] - LeftLine[0]), 
                    TopLine[0] * ((LeftLine[1] - TopLine[1]) / (TopLine[0] - LeftLine[0])) + TopLine[1])

    TopRight = ((RightLine[1] - TopLine[1]) / (TopLine[0] - RightLine[0]), 
                    TopLine[0] * ((RightLine[1] - TopLine[1]) / (TopLine[0] - RightLine[0])) + TopLine[1])

    BottomLeft = ((LeftLine[1] - BottomLine[1]) / (BottomLine[0] - LeftLine[0]), 
                    BottomLine[0] * ((LeftLine[1] - BottomLine[1]) / (BottomLine[0] - LeftLine[0])) + BottomLine[1])

    BottomRight = ((RightLine[1] - BottomLine[1]) / (BottomLine[0] - RightLine[0]), 
                    BottomLine[0] * ((RightLine[1] - BottomLine[1]) / (BottomLine[0] - RightLine[0])) + BottomLine[1])
    
    width, height = 274, 152.5

    if flip_coordinates:
        real_world_points = np.array([
            [width, height, 0], # Top-left
            [0, height, 0],     # Top-right
            [width, 0, 0],      # Bottom-left
            [0, 0, 0]           # Bottom-right
        ], dtype=np.float32)
    else:
        real_world_points = np.array([
            [0, 0, 0],         # Top-left
            [width, 0, 0],     # Top-right
            [0, height, 0],    # Bottom-left
            [width, height, 0] # Bottom-right
        ], dtype=np.float32)

    screen_points = np.array([
        TopLeft, 
        TopRight, 
        BottomLeft, 
        BottomRight
    ], dtype=np.float32)

    #pixel size
    w, h = img.shape[:2]
    pixel_size = sensor_width/h

    #focal length in pixels
    fx = focal_length/pixel_size
    fy = fx

    #principal point
    Cx = h/2
    Cy = w/2


    K = np.array([[fx, 0, Cx],  # fx,  0, cx
                [0, fy, Cy],  #  0, fy, cy
                [0, 0, 1]])  # 0,  0,  1
    
    # 4️⃣ SolvePnP: Find rotation & translation
    success, rvec, tvec = cv.solvePnP(real_world_points, screen_points, K, None)

    # Convert rotation vector to rotation matrix
    R, _ = cv.Rodrigues(rvec)

    # Camera position in world coordinates:
    camera_position = -np.dot(R.T, tvec)
    H, _ = cv.findHomography(screen_points, real_world_points[:, :2])

    return camera_position, H



def track_ball(img, range, name):
    blur = cv.GaussianBlur(img, (15, 15), 0) # blue the image
    hsv = cv.cvtColor(blur, cv.COLOR_BGR2HSV) # convert RGB data to HSV
    
    # get areas of picture in hsv ball range
    mask = cv.inRange(hsv, range[0], range[1])

    # erode and dilate smaller areas that arent the ball
    #mask = cv.erode(mask, None, iterations=2)
    mask = cv.dilate(mask, None, iterations=2)

    # get center of ball
    cnts, _ = cv.findContours(mask.copy(), cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)
    center = None
    if len(cnts) > 0:
        c = max(cnts, key=cv.contourArea)
        ((x, y), rad) = cv.minEnclosingCircle(c)
        M = cv.moments(c)
        center = (int(M['m10']/M['m00']), int(M['m01']/M['m00']))

        if rad > 0: # only show circles big enough
            cv.circle(img, (int(x), int(y)), int(rad), (0, 255, 255), 2)
            cv.circle(img, center, 5, (0, 255, 255), -1)
        cv.imshow(name, img)
        return x,y
    else:
        cv.imshow(name, img)
        return -1,-1
    
    


def main():
    # camera setup
    camera_ip1 = '192.168.0.162'
    camera_ip2 = '192.168.0.204'
    camera_port = 8080
    capture1 = cv.VideoCapture(f'http://{camera_ip1}:{camera_port}/video')
    capture2 = cv.VideoCapture(f'http://{camera_ip2}:{camera_port}/video')

    ballRange = ( (8, 125, 160), (24, 255, 255) ) # tuple with the range of hsv values that are considered a ball (orange range)
    #ballRange = ( (162, 125, 200), (172, 255, 255) )
    

    ret1, img1 = capture1.read()
    ret2, img2 = capture2.read()
    if not ret1 or not ret2: quit()
    camera1_pos, H1 = calibrate_camera(img1, 22, 44.7, False)
    camera1_pos = camera1_pos.ravel()
    camera2_pos, H2 = calibrate_camera(img2, 23, 39.6, True)
    camera2_pos = camera2_pos.ravel()

    while True:
        ret1, img1 = capture1.read()
        ret2, img2 = capture2.read()

        x1, y1 = track_ball(img1, ballRange, "camera 1")
        x2, y2 = track_ball(img2, ballRange, "camera 2")

        pos1 = image_to_world_plane(x1, y1, H1)
        pos2 = image_to_world_plane(x2, y2, H2)

        print(closest_point_between_skew_lines(camera1_pos, camera1_pos - pos1, camera2_pos, camera2_pos - pos2))
        if cv.waitKey(1) & 0xFF == ord('q'): break


if __name__ == "__main__":
    main()
