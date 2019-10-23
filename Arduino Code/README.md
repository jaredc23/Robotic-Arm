# About
These files are the code for the arduinos

# Notes:
Main_Controller.ino:
  This file is for the main Ardunio Mega that contains the touchscreen.
  
Neotrellis_Controller.ino:
  This is for an Arduino Pro Mini which is connected to 2 Adafruit Neotrellis controllers through SDA and SCL pins
  
UpperArm_Controller.ino:
  This is for both the upper arm and lower arm Arduino's despite the name. Next to every constant is a comment with what values should be for each controller
    when I finish the project, I will likely split into two files.
    
 
# Features    
All arduino's communicate over Serial communication, with the NeoTrellis Controller communicating using the Arduino Mega's Software Serial

UpperArm_Controller contains soft limits, as well as supports hard limits. Each motor should be set to its hard limit first, and then no more calibration is required. The motor will never over run where it should be, unless the motor loses steps, to which you must home again

Motor controllers all are autonomous, so all motors can run at the same time. 

Motor controllers support Emergency Stop buttons, for best results tie all the emergency stop pins to the same switch, that goes low when pressed. This will trigger all of them at the same time.
