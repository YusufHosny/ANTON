import pickle
import tkinter as tk
from tkinter import ttk
from comms.UDPServer import UDPServer
import time
from comms.dto_classes import *
import threading as ts
from tracking.Tracker import Tracker
from Visualizer import Visualizer
import cv2 as cv
import math

class ControlPanel:
    def __init__(self, HOST, PORT, automode=False):
        self.server = UDPServer(HOST, PORT)

        self._load_auto = automode

        self._prevstate = None

        self.root = tk.Tk()
        self.root.title("A.N.T.O.N. Control Panel")
        self.root.geometry("400x400")
        
        self.mode = tk.StringVar(value="Slider")
        
        self.button_states = {
            "up": False, "down": False, "left": False, "right": False,
            "fire": False
        }
        self.angle = .5
        
        self.create_widgets()
        self.show_slider_mode()
        
        self.root.bind("<KeyPress-w>", lambda e: self.set_button_state("up", True))
        self.root.bind("<KeyRelease-w>", lambda e: self.set_button_state("up", False))
        
        self.root.bind("<KeyPress-s>", lambda e: self.set_button_state("down", True))
        self.root.bind("<KeyRelease-s>", lambda e: self.set_button_state("down", False))
        
        self.root.bind("<KeyPress-a>", lambda e: self.set_button_state("left", True))
        self.root.bind("<KeyRelease-a>", lambda e: self.set_button_state("left", False))
        
        self.root.bind("<KeyPress-d>", lambda e: self.set_button_state("right", True))
        self.root.bind("<KeyRelease-d>", lambda e: self.set_button_state("right", False))
        
        self.root.bind("<KeyPress-q>", lambda e: self.rotate("ccw"))        
        self.root.bind("<KeyPress-e>", lambda e: self.rotate("cw"))
        
        self.root.bind("<KeyPress-space>", lambda e: self.set_button_state("fire", True))
        self.root.bind("<KeyRelease-space>", lambda e: self.set_button_state("fire", False))
    
    def _load_tracker(self):
        # camera setup
        camera_ip1 = '192.168.137.199'
        camera_ip2 = '192.168.137.68'
        camera_port = 8080
        capture1 = cv.VideoCapture(f'http://{camera_ip1}:{camera_port}/video')
        capture2 = cv.VideoCapture(f'http://{camera_ip2}:{camera_port}/video')

        # capture1 = cv.VideoCapture('tracking/video/camA.mp4')
        # capture2 = cv.VideoCapture('tracking/video/camB.mp4')

        # state = None
        # with open('./state.pkl', 'rb') as f:
        #     state = pickle.load(f)
        ballRange = (10, 160, 160), (24, 255, 255)

        self.tracker = Tracker((capture1, capture2), (22, 23), (44.7, 39.6), ballRange, 'manual', speed='live')

    def _load_visualizer(self):
        self.visualizer = Visualizer()

    def create_widgets(self):
        self.mode_frame = tk.Frame(self.root)
        self.mode_frame.pack(pady=5)
        
        self.slider_btn = tk.Button(self.mode_frame, text="Slider", command=self.show_slider_mode)
        self.slider_btn.pack(side=tk.LEFT, padx=5)
        
        self.keyboard_btn = tk.Button(self.mode_frame, text="Keyboard", command=self.show_keyboard_mode)
        self.keyboard_btn.pack(side=tk.LEFT, padx=5)

        self.auto_btn = tk.Button(self.mode_frame, text="Auto", command=self.show_auto_mode)
        self.auto_btn.pack(side=tk.LEFT, padx=5)
        
        self.main_frame = tk.Frame(self.root)
        self.main_frame.pack(pady=10)
        
        self.reset_btn = tk.Button(self.root, text="Reset", width=10, command=self.server.reset)
        self.reset_btn.pack(side=tk.RIGHT, padx=10, pady=10)

    def show_auto_mode(self):
        self.mode.set("Auto")
        self.clear_frame()
        
        self.horizontal = tk.DoubleVar()
        tk.Label(self.main_frame, text="Horizontal Control").pack()
        self.horizontal_slider = ttk.Scale(self.main_frame, from_=0, to=1, orient="horizontal", variable=self.horizontal)
        self.horizontal_slider.pack(pady=5)
        
        self.vertical = tk.DoubleVar()
        tk.Label(self.main_frame, text="Vertical Control").pack()
        self.vertical_slider = ttk.Scale(self.main_frame, from_=0, to=1, orient="horizontal", variable=self.vertical)
        self.vertical_slider.pack(pady=5)
        
        self.racket_angle = tk.DoubleVar()
        tk.Label(self.main_frame, text="Racket Control").pack()
        self.racket_slider = ttk.Scale(self.main_frame, from_=0, to=180, orient="horizontal", variable=self.racket_angle)
        self.racket_angle.set(90.)
        self.racket_slider.pack(pady=5)
        
        self.fire_btn = tk.Button(self.main_frame, text="Fire")
        self.fire_btn.pack(pady=5)

        self.fire_btn.bind("<ButtonPress>", lambda e: self.set_button_state("fire", True))
        self.fire_btn.bind("<ButtonRelease>", lambda e: self.set_button_state("fire", False))

        if not self.tracker.active: self.tracker.start()
    
    def show_slider_mode(self):
        self.mode.set("Slider")
        self.clear_frame()
        
        self.horizontal = tk.DoubleVar()
        tk.Label(self.main_frame, text="Horizontal Control").pack()
        self.horizontal_slider = ttk.Scale(self.main_frame, from_=0, to=1, orient="horizontal", variable=self.horizontal)
        self.horizontal_slider.pack(pady=5)
        
        self.vertical = tk.DoubleVar()
        tk.Label(self.main_frame, text="Vertical Control").pack()
        self.vertical_slider = ttk.Scale(self.main_frame, from_=0, to=1, orient="horizontal", variable=self.vertical)
        self.vertical_slider.pack(pady=5)
        
        self.racket_angle = tk.DoubleVar()
        tk.Label(self.main_frame, text="Racket Control").pack()
        self.racket_slider = ttk.Scale(self.main_frame, from_=0, to=1, orient="horizontal", variable=self.racket_angle)
        self.racket_slider.pack(pady=5)
        
        self.fire_btn = tk.Button(self.main_frame, text="Fire")
        self.fire_btn.pack(pady=5)

        self.fire_btn.bind("<ButtonPress>", lambda e: self.set_button_state("fire", True))
        self.fire_btn.bind("<ButtonRelease>", lambda e: self.set_button_state("fire", False))
    
    def show_keyboard_mode(self):
        self.mode.set("Keyboard")
        self.clear_frame()
        
        btn_frame = tk.Frame(self.main_frame)
        btn_frame.pack()
        
        self.rotate_ccw_btn = tk.Button(btn_frame, text="↶", command=lambda: self.rotate("ccw"))
        self.rotate_ccw_btn.grid(row=0, column=0, padx=5, pady=5)

        self.rotate_cw_btn = tk.Button(btn_frame, text="↷", command=lambda: self.rotate("cw"))
        self.rotate_cw_btn.grid(row=0, column=2, padx=5, pady=5)
        
        self.up_btn = tk.Button(btn_frame, text="↑")
        self.up_btn.grid(row=0, column=1, padx=5, pady=5)
        
        self.left_btn = tk.Button(btn_frame, text="←")
        self.left_btn.grid(row=1, column=0, padx=5, pady=5)
        
        self.down_btn = tk.Button(btn_frame, text="↓")
        self.down_btn.grid(row=1, column=1, padx=5, pady=5)
        
        self.right_btn = tk.Button(btn_frame, text="→")
        self.right_btn.grid(row=1, column=2, padx=5, pady=5)
        
        self.fire_btn = tk.Button(self.main_frame, text="Fire")
        self.fire_btn.pack(pady=5)

        self.up_btn.bind("<ButtonPress>", lambda e: self.set_button_state("up", True))
        self.up_btn.bind("<ButtonRelease>", lambda e: self.set_button_state("up", False))
        
        self.down_btn.bind("<ButtonPress>", lambda e: self.set_button_state("down", True))
        self.down_btn.bind("<ButtonRelease>", lambda e: self.set_button_state("down", False))
        
        self.left_btn.bind("<ButtonPress>", lambda e: self.set_button_state("left", True))
        self.left_btn.bind("<ButtonRelease>", lambda e: self.set_button_state("left", False))
        
        self.right_btn.bind("<ButtonPress>", lambda e: self.set_button_state("right", True))
        self.right_btn.bind("<ButtonRelease>", lambda e: self.set_button_state("right", False))

        self.fire_btn.bind("<ButtonPress>", lambda e: self.set_button_state("fire", True))
        self.fire_btn.bind("<ButtonRelease>", lambda e: self.set_button_state("fire", False))
    
    def clear_frame(self):
        for widget in self.main_frame.winfo_children():
            widget.destroy()
    
    def set_button_state(self, button, state):
        self.button_states[button] = state
        self.send_async()

    def rotate(self, direction):
        self.angle += 10 if direction == "cw" else -10
        self.angle = min(max(0., self.angle), 180.)
        self.send_async()
    
    def get_state(self):
        return {
            **self.button_states,
            "horizontal": self.horizontal.get(),
            "vertical": self.vertical.get(),
            "angle": self.racket_angle.get()
        }
    
    def start(self):
        self.server.start()
        if self._load_auto: 
            self._load_tracker()
            self._load_visualizer()
            self.tracker.start(visual=True)
            self.visualizer.start()

        time.sleep(2)
        self.packetloop = ts.Thread(target=self.start_sync)
        self.packetloop.start()
        self.root.mainloop()

    def send_async(self):
        state = self.get_state()
        if self._prevstate is not None and self._prevstate == state: return
        self._prevstate = state

        if self.mode.get() == "Keyboard":
            h = 0 if state["left"] else 1 if state["right"] else None
            v = 0 if state["down"] else 1 if state["up"] else None
            self.server.queue.put(
                Packet(
                    StepMessage(StepMessageType.MANUAL if h is not None else StepMessageType.RESET, h if h is not None else 0),
                    StepMessage(StepMessageType.MANUAL if v is not None else StepMessageType.RESET, v if v is not None else 0),
                    RacketMessage(self.angle, state["fire"])
                )
            )
        else:
            raise RuntimeError("Unimplemented, fast mode doesnt work for auto")


    def start_sync(self):
        while self.server.isActive():
            state = self.get_state()
            if self.mode.get() == "Slider":
                self.server.queue.put(
                    Packet(
                        StepMessage(StepMessageType.NORMAL, state["horizontal"]),
                        StepMessage(StepMessageType.NORMAL, state["vertical"]),
                        RacketMessage(state["angle"], state["fire"])
                    )
                )
            elif self.mode.get() == "Auto" and self._load_auto:
                pt = self.tracker.get_point(blocking=True)
                x, y, z = pt

                # feed visualizer
                self.visualizer.queue.put(pt)

                l, w = 274, 152.5 # table dims
                center = l/2, w/2 # center of table

                # set normalized coords
                self.horizontal.set(x/w)
                self.vertical.set(z) # todo properly transform z
                
                # calculate angle to rotate racket by
                dw = self.horizontal.get() - center[1]                    
                theta = math.atan(abs(center[0]/dw))
                angle = math.copysign(90-theta, dw)

                # if close enough fire
                fire = y < l/4

                self.racket_angle.set(angle)

                self.server.queue.put(
                    Packet(
                        StepMessage(StepMessageType.NORMAL, self.horizontal.get()),
                        StepMessage(StepMessageType.NORMAL, self.vertical.get()),
                        RacketMessage(self.racket_angle.get(), fire)
                    )
                )
        
            time.sleep(.3)
        
        

    def stop(self):
        self.server.stop()
        self.packetloop.join()
        if self._load_auto: 
            self.tracker.stop()
            self.visualizer.stop()

if __name__ == "__main__":
    HOST, PORT = '192.168.137.1', 3201 
    app = ControlPanel(HOST, PORT, automode=False)
    app.start()
    app.stop()
