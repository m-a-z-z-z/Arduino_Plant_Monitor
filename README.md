# Smart Garden Monitoring Management System

This system will allow users to configure an Arduino Uno WiFi Rev. 2 device to monitor a plants conditions and health with an accompanying Android application to read the values the sensors retrieve from the plant. Users can add multiple plants via the app to monitor but extra soil moisture sensors will be needed for each plant, or extra Arduinos and accompanying sensors if the plants are in different rooms or locations.
The Arduino and sensors needed can be found at the following links:
- [Arduino](https://ie.farnell.com/arduino/abx00021/development-board-8-bit-avr-mcu/dp/2917573?ost=arduino+uno+wifi)
- [Sunlight sensor](https://ie.farnell.com/seeed-studio/101020089/sunlight-sensor-arduino-raspberry/dp/4007692?ost=arduino+sunlight+sensor)
- [Temperature & Humidity sensor](https://ie.farnell.com/seeed-studio/101020932/sensor-board-arduino-raspberry/dp/4007749?st=arduino%20temperature%20and%20humidity%20sensor)
- [Soil moisute sensor](https://ie.farnell.com/seeed-studio/101020614/sensor-board-soil-moisture-sensor/dp/3932120?st=seeed%20soil%20moisture%20sensor)

## Table of Contents
- [Installation](#installation)
- [Usage](#usage)

## Installation of Android app
 To download the application APK you can head to the latest release on the GitHub project [here.](https://github.com/m-a-z-z-z/Plant_Monitor_App/releases/tag/v1.0.0)
 
 If you'd rather manually compile the application, open your terminal and use the following command to clone the repository:

```bash
git clone https://github.com/m-a-z-z-z/Plant_Monitor_App.git
```

Open the repository in Android Studio, plug your Android device into the computer, and hit run.
[![Screenshot-2023-05-02-014153.png](https://i.postimg.cc/sXnyGfKN/Screenshot-2023-05-02-014153.png)](https://postimg.cc/jwJBGbpH)
## Installation of Arduino
To get the Arduino source code, open your terminal and clone with the following command:
```bash
git clone https://github.com/m-a-z-z-z/Arduino_Plant_Monitor.git
```

Next you will have to
- Download the PlatformIO extension for VSCode
- Open the cloned folder in VSCode and navigate to the "include" folder and add your WiFi SSID and password, as well as your username and plant ID of plant that will be monitored.
- Click "Upload and Monitor" from the extension shortcut as shown in the screenshot below<br>
- [![Screenshot-2023-05-02-015526.png](https://i.postimg.cc/ryxNThy7/Screenshot-2023-05-02-015526.png)](https://postimg.cc/Yj2WfzWx)

## Usage
- Before upload the sketch to the Arduino, you should first register within the Androi application and add a plant.
- Once the plant is added, view the plant by clicking "Select Added Plants" in the dashboard or in the side navigation menu.
- Once in the plant health values screen, make note of the plant ID directly beneath the plant name at the top of the screen. This will need to be added to the secrets.h file, as well as the username you selected in registration.
- [![1682989826244.jpg](https://i.postimg.cc/mkd8fkBB/1682989826244.jpg)](https://postimg.cc/dZy8Gqhx)
- [![1682989826220.jpg](https://i.postimg.cc/8ckt0M8Z/1682989826220.jpg)](https://postimg.cc/YGV644CF)
- [![1682989826206.jpg](https://i.postimg.cc/RCGPnsfy/1682989826206.jpg)](https://postimg.cc/qghcPG1x)
- [![1682989826199.jpg](https://i.postimg.cc/Hx96z3NJ/1682989826199.jpg)](https://postimg.cc/Hcj0s0dH)
- To set up the Arduino plant monitor device with your plant, simply insert the soil moisture sensor into the soil and attach the light and temperature & humidity sensor to the side of the plant pot. 
- [![1682799102889.jpg](https://i.postimg.cc/3RGkQchR/1682799102889.jpg)](https://postimg.cc/Q9X8pSHG)
- Power the Arduino via the USB cable and a power source such as a power bank, or use the DC barrel jack port and plug it into an outlet.
