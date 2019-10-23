These files are the code for the arduinos

Main_Controller.ino:
  This file is for the main Ardunio Mega that contains the touchscreen.
  
Neotrellis_Controller.ino:
  This is for an Arduino Pro Mini which is connected to 2 Adafruit Neotrellis controllers through SDA and SCL pins
  
UpperArm_Controller.ino:
  This is for both the upper arm and lower arm Arduino's despite the name. Next to every constant is a comment with what values should be for each controller
    when I finish the project, I will likely split into two files.
    
    
    
All arduino's communicate over Serial communication, with the NeoTrellis Controller communicating using the Arduino Mega's Software Serial
