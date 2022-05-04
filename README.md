# WOLTER
Wibi Okkel's Laser Turret Entertainment Robot

---
### What's this?
This robot will point a laser around my living room and hallway to entertain my cat (Wibi Okkel).

### Features
WOLTER consists of two servos providing the yaw and pitch movement of a laser module, a 16x2 character LCD screen, an analog joystick and two buttons for interfacing and the entire thing is controlled by an Arduino Uno.

WOLTER has a manual mode, where it can be controlled using the joystick, as well as two automatic modes. In the automatic modes WOLTER will only point the laser on the floor of my living room and hallway, automatically avoiding walls and other obstacles. One mode moves the laserpoint around in straight lines, the other moves it randomly using Perlin noise.

**NOTE**: The automatic modes of WOLTER are designed for avoiding the walls and obstacles specific to the floorplan of my living room and hallway. You're going to have to do your own math if you wish to copy this project.

### Repository contents
* [`WOLTERrbt`](/WOLTERrbt/) has the Arduino code running on the robot.
* [`WOLTERsim`](/WOLTERsim/) has a Processing sketch that simulates the laser movement of the automatic modes.
* [`WOLTERvis`](/WOLTERvis/) has a Processing sketch that will visualise the laser movement received over Serial from an Arduino running the robot code. This allows you to check the Arduino code before building the entire robot.
