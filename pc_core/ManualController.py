import tkinter as tk
from tkinter import ttk
from UDPServer import UDPServer
import time
from dto_classes import *
import threading as ts

class ControlPanel:
    def __init__(self, HOST, PORT):
        self.server = UDPServer(HOST, PORT)

        self.root = tk.Tk()
        self.root.title("A.N.T.O.N. Control Panel")
        self.root.geometry("400x400")
        
        self.mode = tk.StringVar(value="Normal")
        
        self.button_states = {
            "up": False, "down": False, "left": False, "right": False,
            "fire": False
        }
        self.angle = .5
        
        self.create_widgets()
        self.show_normal_mode()
        
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
    
    def create_widgets(self):
        self.mode_frame = tk.Frame(self.root)
        self.mode_frame.pack(pady=5)
        
        self.normal_btn = tk.Button(self.mode_frame, text="Normal", command=self.show_normal_mode)
        self.normal_btn.pack(side=tk.LEFT, padx=5)
        
        self.manual_btn = tk.Button(self.mode_frame, text="Manual", command=self.show_manual_mode)
        self.manual_btn.pack(side=tk.LEFT, padx=5)
        
        self.main_frame = tk.Frame(self.root)
        self.main_frame.pack(pady=10)
        
        self.reset_btn = tk.Button(self.root, text="Reset", width=10, command=self.server.reset)
        self.reset_btn.pack(side=tk.RIGHT, padx=10, pady=10)
    
    def show_normal_mode(self):
        self.mode.set("Normal")
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
    
    def show_manual_mode(self):
        self.mode.set("Manual")
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

    def rotate(self, direction):
        self.angle += .2 if direction == "cw" else -.2
        self.angle = min(max(0., self.angle), 1.)
    
    def poll_buttons(self):
        return self.button_states
    
    def start(self):
        self.server.start()

        def addPacketLoop():
            time.sleep(2)

            while self.server.isActive():
                data = self.poll_buttons()
                if self.mode.get() == "Manual":
                    h = 0 if data["left"] else 1 if data["right"] else None
                    v = 0 if data["down"] else 1 if data["up"] else None
                    self.server.queue.put(
                        Packet(
                            StepMessage(StepMessageType.MANUAL if h is not None else StepMessageType.RESET, h if h is not None else 0),
                            StepMessage(StepMessageType.MANUAL if v is not None else StepMessageType.RESET, v if v is not None else 0),
                            RacketMessage(self.angle, data["fire"])
                        )
                    )
                else:
                    self.server.queue.put(
                        Packet(
                            StepMessage(StepMessageType.NORMAL, self.horizontal.get()),
                            StepMessage(StepMessageType.NORMAL, self.vertical.get()),
                            RacketMessage(self.racket_angle.get(), data["fire"])
                        )
                    )
            
                time.sleep(1)
            
        self.packetloop = ts.Thread(target=addPacketLoop)
        self.packetloop.start()
        self.root.mainloop()

    def stop(self):
        self.server.stop()
        self.packetloop.join()

if __name__ == "__main__":
    HOST, PORT = '192.168.137.1', 3201 
    app = ControlPanel(HOST, PORT)
    app.start()
    app.stop()
