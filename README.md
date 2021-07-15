# ECE 3375B Microprocessors and Microcomputers
PROJECT Code

Project Design of an Embedded System
Problem Definition: Stratospheric Balloons that Connect the World Together

# Main Functionality of the Software
The software should read the data from the pressure and wind speed sensor and use this data
to calculate the actual speed(Vt) and the Offset of the balloon from the original x position.
Also, there is a timer which detects these datas every 0.5 seconds. So every 0.5 seconds the
software will calculate the data by an algorithm which is the function mentioned before to
check if the balloon is at the correct height and direction. And if not, we will use this
calculation’s output to correct the position of the balloon.
