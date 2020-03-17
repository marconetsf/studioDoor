# About the project
This program will conversating via HTTP with a local servidor that will care about recongnize people via OpenCV + python script. When the system recognizes a person it will send a request to an ESP8266 that will control the oppening of the entrance door. 

# About the ESP code

Algorithm - When the ESP8266 working as a WebServer receive a http request it will send de right pulse to control a servo that will moviment the door to open. When the door open, a sensor (that will be installed ahead of the door) will start to captate the presence in a small perimete to recognize when the person left the door range and close it. 

# About the facial recofnition

xxx

# PIR Sensor

The tecnical name of it is HC-SR501, this module can monitoring the presence of moviment in an adjustable range. 
"Once movement is detected the output goes high and stays high only as long as the timer setting dictates, even if there is continuous movement. It then goes low and stays low for a preset period when the sensor is disabled. Then it looks for movement again and goes high when it sees it."

| Specifications | Values |
| -------------  | ------ |
| Voltage | 5 ~ 20 V|
| Output Voltage | 3.3 ~ 0V|
| Sensing Delay | 0.3 ~ 5min|
| Current Limit | 800mA|
| Working Output | 3.3V 500mA | 

| Sensing range | less than 120° |

# Electrical Connection Diagram


![Untitled Sketch 2_bb](https://user-images.githubusercontent.com/37492251/76890456-2b433880-6866-11ea-9393-19c07e237fd9.png)

***Observations about schematich is that this Hi-link component can be replaced by any supply with 5v in the output. 
Theses two resistors are making a voltage divider, that turn 5v in 3.3v (which is te recommended for esp8266)***


