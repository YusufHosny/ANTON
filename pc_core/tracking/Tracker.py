import cv2 as cv
import numpy as np
from typing import List, Tuple, Self, Any, Literal, Dict
from cv2.typing import MatLike
from threading import Thread, Condition
import time


def closest_point_between_skew_lines(p1: np.ndarray, d1: np.ndarray, p2: np.ndarray, d2: np.ndarray) -> np.ndarray:
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
    
    if n_norm == 0: raise ValueError("The lines are parallel or coincident, no unique closest points.")
    
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
    if abs(denom) < 1e-10: raise ValueError("The lines are nearly parallel, numerical instability may occur.")
    
    t = (rhs_d1 * d2d2 - rhs_d2 * d1d2) / denom
    s = (rhs_d1 * d1d2 - rhs_d2 * d1d1) / denom
    
    # Compute closest points
    closest_p1 = p1 + t * d1
    closest_p2 = p2 + s * d2
    
    return (closest_p1 + closest_p2)/2


def image_to_world_plane(u: float, v: float, H: cv.typing.MatLike) -> Tuple[float, float, float]:
    point = np.array([u, v, 1])
    projected = H @ point
    projected = projected / projected[2]  # Normalize
    return projected[0], projected[1], 0.0  # (X, Y, 0)


class Tracker:

    def __init__(self: Self, 
                captures: Tuple[cv.VideoCapture, cv.VideoCapture],
                focal_lengths: Tuple[float, float],
                sensor_widths: Tuple[float, float],
                ballrange: Tuple[Tuple[int, int, int], Tuple[int, int, int]] = None,
                calibrate: Literal['auto', 'manual', 'load'] = 'auto',
                calibration_state: Dict | None = None,
                speed: Literal['live'] | int = 1
                ):
       
        self._captures = captures
        self._focal_lengths = focal_lengths
        self._sensor_widths = sensor_widths
        self._speed = speed

        self._threads: List[Thread] = []

        if ballrange is None: self.range = (8, 125, 160), (24, 255, 255) # default orange ball range
        else: self.range = ballrange
        self._calibration_method = calibrate
        if self._calibration_method == 'load':
            if calibration_state is None:
                raise TypeError('Load Mode selected but no calibration state passed in.')
            self.calibration_state = calibration_state

        self.camera_positions: Tuple[MatLike] = []
        self.H: Tuple[MatLike] = []

        self._pt = None
        self._pt_dirty_bit = Condition()
        self._done = False
        self.active = False

    def start(self: Self, visual: bool = False):
        self._threads += [Thread(target=self._mainloop)]
        self._done = False
        self._visual = visual
        if self._speed == 'live': self._threads += [Thread(target=self._consumeloop)]
        for thread in self._threads:
            thread.start()

    def _consumeloop(self: Self):
        capture1, capture2 = self._captures

        while not self._done:
            ret, img1 = capture1.read()
            if ret: self.img1 = img1

            ret, img2 = capture2.read()
            if ret: self.img2 = img2

            time.sleep(.01)


    def _mainloop(self: Self):
        self.active = True
        capture1, capture2 = self._captures

        if self._calibration_method == 'load':
            self.load_calibration_state(self.calibration_state)
        else:
            self.calibrate()

        while True:
            
            if self._speed == 'live':
                img1 = self.img1
                img2 = self.img2
            else:
                for _ in range(self._speed):
                    _, img1 = capture1.read()
                    _, img2 = capture2.read()

            w1, h1 = img1.shape[:2]
            img1 = cv.resize(img1, (int(800./w1 * h1), 800))
            w2, h2 = img2.shape[:2]
            img2 = cv.resize(img2, (int(800./w2 * h2), 800))

            x1, y1 = self._track(img1, "camera 1")
            x2, y2 = self._track(img2, "camera 2")

            if None not in (x1, x2, y1, y2):

                pos1 = image_to_world_plane(x1, y1, self.H[0])
                pos2 = image_to_world_plane(x2, y2, self.H[1])

                with self._pt_dirty_bit:
                    self._pt = closest_point_between_skew_lines(
                        self.camera_positions[0], self.camera_positions[0] - pos1, 
                        self.camera_positions[1], self.camera_positions[1] - pos2
                        )
                    self._pt_dirty_bit.notify()
            
            if cv.waitKey(1) & 0xFF == ord('q') or self._done:
                break

        self._done = True
        self.active = False
        with self._pt_dirty_bit:
            self._pt_dirty_bit.notify_all()
    
    def stop(self: Self):
        self._done = True
        for thread in self._threads:
            thread.join()

    def get_point(self: Self, blocking: bool = False):
        if blocking and self.active:
            with self._pt_dirty_bit: self._pt_dirty_bit.wait()
        return self._pt

    def _points_to_bounds(self: Self, clicks: List[Tuple[int, int]]) -> Tuple[int, int, int, int]:
        if len(clicks) != 4: raise ValueError('Incorrect number of points')

        # convert clicks into bounding coordinates (top, bottom, left, right)
        t = min(map(lambda c: c[1], clicks)) # min of y coordinates
        b = max(map(lambda c: c[1], clicks)) # max of y coordinates
        l = min(map(lambda c: c[0], clicks)) # min of x coordinates
        r = max(map(lambda c: c[0], clicks)) # max of x coordinates

        return t, b, l, r

    # query user for 4 points to mask inner area of the table
    def _mask_center(self: Self, img: MatLike):
        clicks = [] # list of 4 points that the user clicks
        def add_click(event: int, x: int, y: int, flags: int, param: Any | None) -> None:
            nonlocal clicks
            if event == cv.EVENT_LBUTTONUP:
                clicks += [(x, y)]
        
        # show original frame, let user click on 4 points to define inner "ignore mask" 
        cv.namedWindow("Original")
        cv.setMouseCallback("Original", add_click)
        cv.imshow("Original", img)
        while len(clicks) < 4:
            cv.waitKey(500)
        cv.destroyWindow("Original")

        # get mask and show to user
        mask = self._get_table_mask(img, clicks)
        masked_img = cv.bitwise_and(img, img, mask=mask)
        cv.imshow("masked image", masked_img)
        cv.waitKey(0)
        cv.destroyWindow("masked image")

        return masked_img
    
    # generate mask based on 4 points
    def _get_table_mask(self: Self, img: MatLike, clicks: List[Tuple[int, int]]) -> MatLike:
        t, b, l, r = self._points_to_bounds(clicks)

        # generate mask based on bounds
        (h, w) = img.shape[:2]
        mask = np.zeros((h, w), np.uint8)
        cv.rectangle(mask,(0, int(b)), (w, int(t)), 255, -1)
        cv.rectangle(mask,(int(l), int(b - 60)),(int(r), int(t + 60)), 0, -1)

        return mask
    
    def _get_corners_auto(self: Self, img: MatLike, flip_coordinates: bool) -> Tuple[np.ndarray, np.ndarray]:
        # WHITE COLOR BOUNDARIES
        lower_bound = (0, 0, 200)
        upper_bound = (179, 110, 255)

        # image width and height
        w, h = img.shape[:2]
        
        # mask range and run hough transform to search for lines
        hsv = cv.cvtColor(img, cv.COLOR_BGR2HSV)
        mask = cv.inRange(hsv, lower_bound, upper_bound)
        Plines = cv.HoughLinesP(mask, 1, np.pi/180, 100, None, 150, 50)
        if Plines is None: raise ValueError('Hough detected no lines')

        # transform mask to rgb
        cdst = cv.cvtColor(mask, cv.COLOR_GRAY2BGR)

        # populate list of lines
        BottomLines = [], []
        TopLines    = [], []
        LeftLines   = [], []
        RightLines  = [], []
        for line in Plines:
            l = line[0]
            slope = (l[3]-l[1]) / (l[2]-l[0])
            y_intercept = l[1] - slope * l[0]

            if abs(slope) < 0.4:
                if l[1] < w/2:
                    TopLines[0].append(slope)
                    TopLines[1].append(y_intercept)
                else:
                    BottomLines[0].append(slope)
                    BottomLines[1].append(y_intercept)
            else:
                if slope < 0:
                    LeftLines[0].append(slope)
                    LeftLines[1].append(y_intercept)
                else:
                    RightLines[0].append(slope)
                    RightLines[1].append(y_intercept)

            cv.line(cdst, (l[0], l[1]), (l[2], l[3]), (0,0,255), 3, cv.LINE_AA)
        cv.namedWindow('Detected Lines (in red) - Probabilistic Line Transform')
        cv.imshow("Detected Lines (in red) - Probabilistic Line Transform", cdst)
        cv.waitKey(0)
        cv.destroyWindow("Detected Lines (in red) - Probabilistic Line Transform")
        
        # average out lines per group
        def avg(iterable): return sum(iterable)/len(iterable)
        TopLine     = avg(TopLines[0]),     avg(TopLines[1])
        BottomLine  = avg(BottomLines[0]),  avg(BottomLines[1])
        LeftLine    = avg(LeftLines[0]),    avg(LeftLines[1])
        RightLine   = avg(RightLines[0]),   avg(RightLines[1])

        # derive corners from lines by solving for intersection
        # y = m1*x + b1 and y = m2*x + b2
        # x = (b2 - b1) / (m1 - m2) 
        def solve_corner(line1: Tuple[float, float], line2: Tuple[float, float]) -> Tuple[float, float]:
            m1, b1 = line1
            m2, b2 = line2

            x = (b1 - b2) / (m2 - m1)
            y = m1 * x + b1

            return x, y

        TopLeft     = solve_corner(LeftLine, TopLine)
        TopRight    = solve_corner(RightLine, TopLine)
        BottomLeft  = solve_corner(LeftLine, BottomLine)
        BottomRight = solve_corner(RightLine, BottomLine)
        
        # define real points and screen space point arrays
        length, width = 274, 152.5  # concrete length and width of ping pong table

        real_world_points = np.array([
                [0, 0, 0],         # Top-left
                [length, 0, 0],     # Top-right
                [0, width, 0],    # Bottom-left
                [length, width, 0] # Bottom-right
            ], dtype=np.float32)
        if flip_coordinates: real_world_points = np.flip(real_world_points, 0)

        screen_points = np.array([
            TopLeft, 
            TopRight, 
            BottomLeft, 
            BottomRight
        ], dtype=np.float32)

        return real_world_points, screen_points

    def _get_corners_manual(self: Self, img: MatLike, flip_coordinates: bool) -> Tuple[np.ndarray, np.ndarray]:
        clicks = [] # list of 4 points that the user clicks
        def add_click(event: int, x: int, y: int, flags: int, param: Any | None) -> None:
            nonlocal clicks
            if event == cv.EVENT_LBUTTONUP:
                clicks += [(x, y)]
        
        # show original frame, let user click on 4 points to define corners
        cv.namedWindow("Original")
        cv.setMouseCallback("Original", add_click)
        cv.imshow("Original", img)
        while len(clicks) < 4:
            cv.waitKey(500)
        cv.destroyWindow("Original")

        # get corners
        t, b, l, r = self._points_to_bounds(clicks)
        
        TopLeft     = l, t
        TopRight    = r, t
        BottomLeft  = l, b
        BottomRight = r, b

        # define real points and screen space point arrays
        length, width = 274, 152.5  # concrete length and width of ping pong table

        real_world_points = np.array([
                [0, 0, 0],         # Top-left
                [length, 0, 0],     # Top-right
                [0, width, 0],    # Bottom-left
                [length, width, 0] # Bottom-right
            ], dtype=np.float32)
        if flip_coordinates: real_world_points = np.flip(real_world_points, 0)

        screen_points = np.array([
            TopLeft, 
            TopRight, 
            BottomLeft, 
            BottomRight
        ], dtype=np.float32)

        return real_world_points, screen_points

    def _solve_camera_matrix(self: Self, img: MatLike, focal_length: float,sensor_width: float, real_world_points: np.ndarray, screen_points: np.ndarray) -> Tuple[MatLike, MatLike]:
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

    def calibrate(self: Self):
        capture1, capture2 = self._captures

        ret1, img1 = capture1.read()
        ret2, img2 = capture2.read()
        if not ret1 or not ret2: quit()

        w1, h1 = img1.shape[:2]
        img1 = cv.resize(img1, (int(800./w1 * h1), 800))
        w2, h2 = img2.shape[:2]
        img2 = cv.resize(img2, (int(800./w2 * h2), 800))

        camera1_pos, H1 = self._calibrate_single(img1, self._focal_lengths[0], self._sensor_widths[0], False)
        camera1_pos = camera1_pos.ravel()
        camera2_pos, H2 = self._calibrate_single(img2, self._focal_lengths[1], self._sensor_widths[1], True)
        camera2_pos = camera2_pos.ravel()

        self.camera_positions = camera1_pos, camera2_pos
        self.H = H1, H2

    def export_calibration_state(self: Self) -> str | None:
        if any((len(self.H) == 0, len(self.camera_positions) == 0)):
            return None
        
        return {
            'H': self.H,
            'camera_positions': self.camera_positions
        }
    
    def load_calibration_state(self: Self, state: str):
        if None in (state, state['H'], state['camera_positions']):
            raise TypeError('Invalid Calibration State')

        self.H = state['H']
        self.camera_positions = state['camera_positions'] 



    def _calibrate_single(self: Self, img: MatLike, focal_length: float, sensor_width: float, flip_coordinates: bool) -> Tuple[MatLike, MatLike]:

        if self._calibration_method == 'auto':
            # query user and mask inner rect on frames
            img = self._mask_center(img)
            real_world_points, screen_points = self._get_corners_auto(img, flip_coordinates)
        else:
            real_world_points, screen_points = self._get_corners_manual(img, flip_coordinates)

        return self._solve_camera_matrix(img, focal_length, sensor_width, real_world_points, screen_points) 

    def _track(self: Self, img: MatLike, name: str) -> Tuple[float, float] | Tuple[None, None]:
        
        blur = cv.GaussianBlur(img, (15, 15), 0) # blur the image
        hsv = cv.cvtColor(blur, cv.COLOR_BGR2HSV) # convert RGB data to HSV
        mask = cv.inRange(hsv, self.range[0], self.range[1]) # get areas of picture in hsv ball range
        mask = cv.dilate(mask, None, iterations=4) # dilate mask

        # get center of ball
        cnts, _ = cv.findContours(mask.copy(), cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)
        center = None
        if len(cnts) > 0:
            c = max(cnts, key=cv.contourArea)
            ((x, y), rad) = cv.minEnclosingCircle(c)
            M = cv.moments(c)
            center = (int(M['m10']/M['m00']), int(M['m01']/M['m00']))

            cv.circle(img, (int(x), int(y)), int(rad), (0, 255, 255), 2)
            cv.circle(img, center, 5, (0, 255, 255), -1)
        else:
            x, y = None, None

        if self._visual: cv.imshow(name, img)
        return x, y

