# Chicken Little 3000 🐣

[Leia este README em Português](README_PT-BR_.md)

The Chicken Little 3000 is an automated incubator designed to control temperature and humidity during the incubation process of quail and chicken eggs. The system uses sensors to maintain optimal conditions for hatching eggs, ensuring a higher success rate. 🌡️💧

![Chicken Little 3000](assets/images/img_01.png)

## Objective 🎯

This project is the first interdisciplinary project of the undergraduate course in Control and Automation Engineering. It was developed as part of the academic experience to integrate knowledge from various engineering disciplines, including microcontroller programming, sensor integration, and 3D modeling. 🔧📐

## Features 🌟

- **Temperature and Humidity Control**: The system uses a DHT22 sensor to measure and control both temperature and humidity inside the incubator.
- **ESP32 Microcontroller**: The system is powered by the ESP32, a versatile and efficient microcontroller, for real-time control and monitoring. ⚡
- **3D Printing**: The structure of the incubator is 3D printed, providing flexibility in design and efficient use of materials. 🖨️
- **Laser-Cut MDF Structure**: The external structure is made of MDF, cut by laser, ensuring precision and durability. 🔥

## Components ⚙️

- **ESP32 Microcontroller**: Main controller for the incubator system.
- **DHT22 Sensor**: Measures the temperature and humidity.
- **3D Printed Parts**: Custom parts created through 3D printing for the internal structure.
- **Laser-Cut MDF**: External case made of MDF cut by laser.

## Getting Started 🚀

### Prerequisites 📋

- **Hardware**:
  - ESP32 Microcontroller
  - DHT22 Temperature and Humidity Sensor
  - 3D Printed Parts
  - Laser-Cut MDF

- **Software**:
  - Arduino IDE or PlatformIO for programming the ESP32
  - Libraries for DHT22 sensor and ESP32 (available on the Arduino IDE Library Manager)

### Installation ⚡

1. Install the necessary libraries in the Arduino IDE.
2. Upload the code to the ESP32.
3. Connect the DHT22 sensor to the appropriate pins of the ESP32.
4. Power on the system and observe the temperature and humidity regulation.

## How It Works 🔍

The Chicken Little 3000 uses the ESP32 microcontroller to monitor and regulate the temperature and humidity levels inside the incubator. The DHT22 sensor continuously measures the environment, and the system adjusts the heating and humidity levels to maintain optimal conditions for egg incubation. 🥚

## Contributing 🤝

If you'd like to contribute to this project, feel free to fork the repository, create a branch, and submit a pull request with your changes.

## License 📜

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

## Acknowledgments 🙏

- **ESP32**: For its versatility and power in managing sensors and controlling the system.
- **DHT22**: For providing reliable and accurate temperature and humidity readings.
- **3D Printing**: For enabling custom part creation at low cost.
- **Laser-Cut MDF**: For creating the precise external structure of the incubator.
