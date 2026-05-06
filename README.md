# stm32g431-hal

STM32G431RBTx (Cortex-M4, 128KB Flash / 32KB SRAM) embedded application for the CT117E-M4 development board, built with STM32CubeMX HAL and Keil MDK.

## Hardware

- **MCU:** STM32G431RBTx (Cortex-M4 @ 160 MHz)
- **Board:** CT117E-M4
- **Peripherals:** 240×320 TFT LCD (ILI9341), 4 buttons, 8 LEDs, AT24C02 EEPROM, MCP40xx digital pot (software I2C)

## Features

| Module | Description |
|--------|-------------|
| ADC | Dual ADC sampling with voltage readout |
| DAC | 2-channel 0–3.3V output |
| PWM | TIM16/TIM17 PWM (10–90% duty) |
| Input Capture | TIM2/TIM3 frequency & duty measurement |
| Software I2C | Bit-banged PB6/PB7 for EEPROM & digital pot |
| LCD | 240×320 TFT, 16-bit parallel interface |
| USART | printf redirect at 9600 baud |

## Build

Open `MDK-ARM\HAL.uvprojx` in Keil uVision 5.35+ (ARM Compiler 5). The project outputs `MDK-ARM\HAL\HAL.hex`.

Compiler defines: `USE_HAL_DRIVER;STM32G431xx`

## Architecture

STM32CubeMX-generated project. Custom code resides between `USER CODE BEGIN` / `USER CODE END` markers. A page-based UI framework (`Src/page/page_manager.c`) manages the LCD interface.

## License

MIT
