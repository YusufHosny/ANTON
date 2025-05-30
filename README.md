# The Autonomous Nemesis for Table Tennis with Optimized Navigation (ANTON)
in this project within the context of IB3 at groupT, we developed ANTON, an autonomous adversarial ping pong robot capable of autonomous ball tracking and navigation, built on a generic gantry subsystem that we developed, which can be controlled via a platform/implementation agnostic UDP stream interface. The interface is connected to a python gui controller which acts as the connectivity layer between the tracking and control systems.

The system incorporates a 2 camera ball-tracking system to determine the ping pong ball's location in real-time and enable reactive ping pong gameplay. The tracking system is generic and robust, written in python with opencv, and can be used separately for other downstream tasks. A Visualizer is implemented with matplotlib, to allow for the ball position to be actively visualized during the system's operation.

Further architctural and implementation details can be found in the paper within the repository.

