# STM32F446RET6 Firmware Repository

## Description
This repository contains the firmware for the STM32F446RET6 multifunctional development board, developed by Nguyen Tuan Anh and contributed by Nguyen Quoc Trung as part of an undergraduate thesis. The firmware is built using the FreeRTOS API and integrates micro-ROS utilities from [micro_ros_stm32cubemx_utils](https://github.com/micro-ROS/micro_ros_stm32cubemx_utils).

## Installation
To clone the repository, run: `git clone https://github.com/trungnguyen0503/micro_ros_firmware.git`

## Setup Instructions
1. **Prerequisites**:
   - Install [STM32CubeIDE](https://www.st.com/en/development-tools/stm32cubeide.html) or a compatible IDE.
   - Install [ROS 2 Humble Hawksbill](https://docs.ros.org/en/humble/Installation.html) on Ubuntu 22.04 LTS.
   - Install the micro-ROS build system as per [micro-ROS documentation](https://micro.ros.org/docs/).

2. **Build the Firmware**:
   - Navigate to the cloned repository: `cd micro_ros_firmware`
   - Open the project in STM32CubeIDE or use the provided build scripts (e.g., `make`).
   - Configure the micro-ROS agent and build the firmware following the instructions in [micro_ros_stm32cubemx_utils](https://github.com/micro-ROS/micro_ros_stm32cubemx_utils).

3. **Flash the Firmware**:
   - Connect the STM32F446RET6 board via a debugger (e.g., ST-Link).
   - Flash the compiled firmware using STM32CubeIDE or a tool like [STM32CubeProgrammer](https://www.st.com/en/development-tools/stm32cubeprog.html).

## Example Output
Below is a screenshot of the firmware running on the STM32F446RET6 board:

![Firmware Screenshot](images/screenshot.png)

## Contributing
Contributions are welcome! To contribute to this project, please follow these steps:
1. **Fork the Repository**: Click the "Fork" button on GitHub to create a copy of this repository under your account.
2. **Clone Your Fork**: `git clone https://github.com/YOUR_USERNAME/micro_ros_firmware.git`
3. **Create a Branch**: `git checkout -b feature/your-feature-name`
4. **Make Changes**: Implement your changes or fixes, ensuring they align with the project’s coding standards.
5. **Test Your Changes**: Verify that the firmware builds and runs correctly on the STM32F446RET6 board.
6. **Commit and Push**:
   ```bash
   git commit -m "Add your commit message here"
   git push origin feature/your-feature-name
