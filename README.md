# DCMotorSpeedControlWithFuzzyLogic
The robot following the line was developed using fuzzy logic to control the bends and slopes in a controlled way.

ABSTRACT
The subject of the project I'm working on is to provide dc motor speed control using fuzzy logic. The car reduces its speed in cornering, increases its speed when it goes uphill, and decreases its speed when it goes downhill. With the sensor I attached to the Arduino, I get the values of tilt and bend angle. The sensor I use is the Accelerometer and Gyroscope sensor. I use 10 slope and 5 cornering parameters as fuzzy logic input parameters. In total there are 50 membership functions. By sending the values I get from the sensor to the fuzzy logic membership functions, I get the dc motor speed value as the output value. Using fuzzy logic, I send the speed values to DC motors by using PWM signal with arduino. So I realized dc motor speed control using fuzzy logic.

1.	INTRODUCTION
The purpose of this work is to provide dc motor speed control using fuzzy logic. The dc engine was run on a line tracking robot to provide speed control. With this work, the robot that follows the line will provide the speed on its own during the bends and on the slopes. In this way, a robot that follows the line using fuzzy logic will be developed. When the robot is cornering or on a sloping road, the parameters needed to set the speed are retrieved using the sensor. These parameters are sent to fuzzy logic membership functions and the speed value of the robot is taken  as output. This speed value is transmitted  to the dc motors via the motor drive using the PWM signal. These operations are carried out instantaneously. As a result of these operations, the speed of the robot is adjusted  according to the road condition.

2.	METHOD
Using fuzzy logic, the speed control of the dc motor has been exploited from various components to perform on the robot following the line. Components used: Arduino mega 2560 r3, MPU6050 6 axis gyro / accelerometer, 2 QTR-3A line sensor, 2 DC motor set, L298N motor driver, 9v Li-po battery, 3 leds, cables, robot body, drunk wheel .

2.1 Arduino Mega 2560
The Arduino Mega 2560 is an Arduino card with an ATmega2560 microcontroller. Coding is done with Arduino IDE. It can be said that Arduino is the most preferred Arduino card after Uno. It has exactly the same features as the Genuino Mega 2560 card, which carries the Genuino brand, the sister brand of Arduino. The Arduino Mega 2560 has 54 digital input / output pini. 15 of them can be used as PWM output. There are also 16 analog inputs, 4 UART (hardware serial port), one 16 MHz crystal oscillator, USB connection, power jack (2.1mm), ICSP header and reset button. The Arduino Mega 2560 includes all of the components required to support a microcontroller. The Arduino Mega 2560 can be connected to a computer, powered by an adapter or battery. Arduino Mega can be used with most of the shields designed for Arduino Duemilanove or Diecimila.

2.2	MPU6050
The Mpu6050 can be operated with a supply voltage of 3 to 5 V. Both accelerometer and gyro outputs give I²C output from separate channels. Each axis can output with a resolution of 16 bits. Gyro measurement range: + 250 500 1000 2000 ° / s. This measurement range has been reduced to 0-180 ° with the "map" function on the code. Angular accelerometer measurement range: ± 2 ± 4 ± 8 ± 16 g. By applying the same procedure for gyro, the angle value is reduced to 0-180 °. The z-axis of the gyro axis is used for the cornering angle. For the slope angle, the x-axis is used for the angular acceleration axes. These values from the sensor are sent to the fuzzy logic membership functions and the speed value is taken as output.

2.3	QTR-3A
The QTR-3A reflection sensor carries a single infrared LED and a phototransistor pair. The on-photo-transistor calculates the IR analog values, which are reflected on the microcontroller by using the digital input / output line using a capacitor discharge circuit, by measuring the discharging time of the capacitor. It produces analog value as output. Faster capacitor discharge times mean  more reflection. The values obtained from this sensor enable the robot to follow the line.

2.4	DC Motors
The operating voltage of the motors is between 3-6V. Speed is 250 rpm. It works according to the voltage coming from the motor driver.

2.5	L298N Motor Driver
Prepared for driving motors up to 24V, this motor driver card has two channels and delivers 2A current per channel. L298N motor driver integration is used on the card. There are two enable pins. With these pins, two separate dc motors can be operated. There is a 5V exit pin. This pin can feed the arduino.

2.6	9V Battery
The batteries have rechargeable 9v 700mA power. The motor drive is running with the power supply. The arduino is supplied from the 5v output pin on the motor drive.

2.7	Leds
The Leds are positioned next to the line sensors. Thanks to the Leds, healthy work is provided even in dark places without light.
All the components are combined to provide synchronized operation of the line tracking robot. The line sensors follow the black line. The values from the line sensors determine which motor should work. When the sensor on the right sees a line, the motor on the left is given power. So the line is centered. Likewise, when you see other sensors, it is decided which motor will work. So the robot turns its curves. It measures the angle of rotation and tilt angle with the mpu6050 sensor on the way. These values are sent to the fuzzy logic. The output speed value is sent to the motor driver as the pwm signal. The motor driver powers the engines according to this value. Thus, dc motor speed control is achieved by using fuzzy logic.
