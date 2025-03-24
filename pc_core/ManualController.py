import tkinter as tk
from tkinter import DoubleVar, IntVar, BooleanVar
from UDPServer import *

srv = UDPServer('192.168.137.1', 3201)
srv.start()

def addPacketLoop():
    time.sleep(2)
    while srv.isActive():
        srv.queue.put(
            Packet(
                StepMessage(activate_var.get(), horizontal_var.get(), urgency_var.get()),
                StepMessage(activate_var.get(), vertical_var.get(), urgency_var.get()),
                RacketMessage(racket_var.get(), fire_var.get())
            )
        )
        time.sleep(1)
    
packetloop = ts.Thread(target=addPacketLoop)

root = tk.Tk()
root.title("Manual ANTON Control Panel")
root.geometry("500x400")

# Restart Server
reset_button = tk.Button(root, text="Restart Server", command=srv.reset)
reset_button.pack()

# Horizontal Module
activate_var = BooleanVar()
activate_button = tk.Checkbutton(root, text="Activate Horizontal Module", variable=activate_var)
activate_button.pack()

horizontal_var = DoubleVar()
tk.Label(root, text="Horizontal Extent").pack()
horizontal_slider = tk.Scale(root, from_=0, to=1, resolution=0.01, orient="horizontal", variable=horizontal_var)
horizontal_slider.pack()

# Vertical Module
activate_var = BooleanVar()
activate_button = tk.Checkbutton(root, text="Activate Vertical Module", variable=activate_var)
activate_button.pack()

vertical_var = DoubleVar()
tk.Label(root, text="Vertical Extent").pack()
vertical_slider = tk.Scale(root, from_=0, to=1, resolution=0.01, orient="horizontal", variable=vertical_var)
vertical_slider.pack()

# Urgency
urgency_var = IntVar()
tk.Label(root, text="Urgency").pack()
urgency_entry = tk.Entry(root, textvariable=urgency_var)
urgency_entry.pack()

# Racket Module
racket_var = DoubleVar()
tk.Label(root, text="Racket Angle").pack()
racket_slider = tk.Scale(root, from_=0, to=1, resolution=0.01, orient="horizontal", variable=racket_var)
racket_slider.pack()

fire_var = BooleanVar()
fire_button = tk.Checkbutton(root, text="Fire", variable=fire_var)
fire_button.pack()


# start loops
packetloop.start()
root.mainloop()

# collect threads
srv.stop()
packetloop.join()
