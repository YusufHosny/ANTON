import numpy as np
import time
import multiprocessing as mp

from matplotlib import pyplot as plt
from matplotlib.patches import Rectangle
from mpl_toolkits.mplot3d import art3d
from matplotlib.animation import FuncAnimation


X, Y, Z = 0, 1, 2
class Visualizer:

    def __init__(self):
        self._ball = np.zeros((3, 1))
        self.queue = mp.Queue()
        self.done = False

    def start(self):
        self._done = False
        self._proc = mp.Process(target=self._mainloop)
        self._proc.start()
        
        
    def _mainloop(self):
        def animate(frame,):
            nonlocal self
            self._update_ball_plot()

        self._fig = plt.figure()
        self._ax = self._fig.add_subplot(111, projection='3d')

        self.initialize_table_gui(self._ax)

        self._ani = FuncAnimation(self._fig, animate, frames=np.arange(0, 360, 5), interval=100, blit=False)

        plt.show()

            

    def initialize_table_gui(self, ax):
        length = 274
        width = 152.5
        net_height = 20

        base_table1 = Rectangle((0, 0), length/2, width)
        art3d.pathpatch_2d_to_3d(base_table1, z=0, zdir="z")
        base_table2 = Rectangle((length/2, 0), length/2, width)
        art3d.pathpatch_2d_to_3d(base_table2, z=0, zdir="z")

        net_table = Rectangle((0, 0), width, net_height, facecolor='none', edgecolor='w', hatch='xxxxx')
        art3d.pathpatch_2d_to_3d(net_table, z=length/2, zdir="x")
        net_table.set_zorder(10)

        ax.add_patch(base_table1)
        ax.add_patch(base_table2)
        ax.add_patch(net_table)

        self._ball_plot = ax.scatter(1, 1, 1, color='yellow', s=10)

        ax.set_xlim(0, length)
        ax.set_ylim(0, length)
        ax.set_zlim(0, length)

        


    def stop(self):
        self._done = True
        self._proc.terminate()
        self._proc.join()

    def _update_ball_plot(self):
        try:
            self._ball = self.queue.get_nowait()
        except: 
            pass

        if self._ball_plot:
            self._ball_plot.remove()

        self._ball_plot = self._ax.scatter(self._ball[X], self._ball[Y], self._ball[Z], color='yellow', s=10)




def main():
    import threading as ts
    tmax = 10

    steps = int(tmax/.2)
    x = np.linspace(0, 100, steps)
    y = np.linspace(0, 200, steps)
    z = 30 * np.abs(np.sin(np.linspace(0, 3 * np.pi, steps)))

    positions = np.column_stack((x, y, z)) 

    table = Visualizer()

    def feed():
        nonlocal table
        for position in positions:
            table.queue.put(position)
            time.sleep(.2)
    feedt = ts.Thread(target=feed)
    feedt.start()

    table.start()
    time.sleep(tmax)
    table.stop()
    
    feedt.join()

if __name__ == '__main__':
    main()
