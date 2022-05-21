# CS2600_FinalProject
This is the final project for Spring 2022 CS2600 with Professor Davarpanah

The purpose of the project is to learn to use the ESP32 and integrate MQTT with it.
Competence is demonstrated in two parts:

Part 1: Using MQTT to remotely enable or disable an LED connected to the ESP32 at the press 
of either 1 or 2 on the remote console.

Part 2: Create a program that automates calls to the ESP32 via MQTT. This part was given
as a free response problem of sorts.

For part 2 I chose to create a digital balance called digiBalance. 
It makes use of an MPU6050 accelerometer to perform the measurements.
An 10-LED light bar is used to reflect how far off balance in the y-direction the balance is.
An LCD screen is used to reflect how far off balance in the x-direction the balance is.
The LCD screen also provides the current angle, in degrees, of both the x and y directions.
The ESP32 on board uses MQTT to execute remote commands.

Commands:
angle - sets the digiBalance to the default mode of measuring the angle in degrees.

findx "number" - Guides the user with audio feedback when the balance is within +- 3 degrees of the target angle.

findy "number" - Does the same as above, but in the y-direction.

feedval - Broadcasts the current reading of the device to a remote console and subscriber of digiBalance/feed

cintv - Changes the interval at which the device calculates (and broadcasts) the measurements.

Video demonstration:
https://youtu.be/xwn6cSpX4pE
