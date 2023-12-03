# Data Acquisition and Storage System Project Report

## Introduction
The project centered around utilizing the STM32F103C8T6 32-bit ARM microcontroller in conjunction with FreeRTOS as the Operating System. 
The primary objective was to develop a program that would capture measurements from the LIS3MDL sensor and store them in the W25Q80DV memory, 
assigning a unique identifier to each measurement. This system enables querying specific measurements via UART using their corresponding IDs. 
Additionally, pressing a button will retrieve the last stored measurement.

## Configuration of Components
- **Microcontroller:** STM32F103C8T6, chosen as the central processing unit.
- **Operating System:** FreeRTOS, used for task management and synchronization.
- **Sensors:** 
  - LIS3MDL, initialized and configured using the I2C protocol.
  - W25Q80DV, configured with software protection and communicated through SPI.

## Technical Details
- **Communication**: 
  - W25Q80DV: SPI interface used for data transfer with the memory.
  - LIS3MDL: Communication via the I2C protocol for configuration and measurement acquisition.
- **Libraries Used**: HAL libraries provided by STM32 were employed to ease development and interaction with components.
