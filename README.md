# STM32 Player

## Authors

- Jakub Kłoczko
- Michał Zelek 

## Description

    Project of music player built with STM32F411E-Discovery and Raspberry Pico W.
    The STM board reads MP3 files from SD Card, decodes MP3 and sends data to
    DAC. Through the Pico board, the control of the STM board takes place.

## STM32

    Board was programmed with STM32CubeIDE, peripherals were initialized
    by CubeMX tool. The real-time system freeRTOS is responsible for controlling the board.

- RTOS [freeRTOS](https://github.com/MikeZ7/MP3_Player/tree/master/STM_music/Middlewares/Third_Party/FreeRTOS/Source)
- Reading SD Card - [FATFS](https://github.com/MikeZ7/MP3_Player/tree/master/STM_music/Middlewares/Third_Party/FatFs/src)
- Decoding MP3 files - [helix](https://github.com/MikeZ7/MP3_Player/tree/master/STM_music/Drivers/helix)
- DAC initialization and control - [Audio](https://github.com/MikeZ7/MP3_Player/tree/master/STM_music/Application/Audio)
- Communication with Pico - UART
- Volume and pause control - incremental encoder

## Pico

    Board is responsible for controling volume and pausing music using UART interface.
    Current music volume and title is printed on OLED display via I2C intefrace.
    Commands are sent by WIFI from web UI.

- OLED initialization and control

## Components

- STM32F411-Discovery
- Raspberry Pico W
- OLED I2C display
- SD Card Reader
- Rotary encoder
- Headphones

[![YouTube Video](https://i3.ytimg.com/vi/xjDPaceiwWM/maxresdefault.jpg)]("https://www.youtube.com/watch?v=xjDPaceiwWM&ab_channel=JakubK%C5%82oczko")
  
![player](https://github.com/MikeZ7/MP3_Player/blob/master/Images/mp3_player.jpg)
