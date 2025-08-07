# SmartThings IoT Environmental Sensor

## Hardware Components

- ESP microcontrollers (ESP8266, ESP32 or ESP32-C3)
- BME280 environmental sensor
- Button for user interaction

## Getting started

#### 1. Set Up SmartThings Project 

1. Go to [SmartThings Developer Workspace](https://developer.smartthings.com/workspace/)

2. Create a new project following the [Getting Started](https://github.com/SmartThingsCommunity/st-device-sdk-c/blob/main/doc/getting_started.md)

3. In Device Profile, add these components:
- Temperature Measurement
- Relative Humidity Measurement

#### 2. Clone repository

```bash
git clone --recurse-submodules https://github.com/MaxDevHub/iot-env-sensor.git
```

#### 3. Add Configuration Files

Place these files in the `main` folder
- `device_info.json`
- `on_boarding_config.json`

#### 4. Build and flash

```bash
source ${IDF_PATH}/export.sh 
idf.py build 
idf.py flash
```
