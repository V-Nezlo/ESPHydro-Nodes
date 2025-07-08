/*!
@file
@brief Аппер
@author V-Nezlo (vlladimirka@gmail.com)
@date 11.05.2024
@version 1.0
*/

#include "GpioWrapper.hpp"
#include "SerialWrapper.hpp"
#include "RsUpper.hpp"
#include "UpperSensors.hpp"
#include "Crc8.hpp"

#include <Arduino.h>

Gpio latch(2, OUTPUT);
Gpio led(10, OUTPUT);
SerialWrapper serial(115200, latch, led);

Gpio acSensePin(A0, INPUT);
Gpio floatLevelPin(A1, INPUT);

Gpio damPin(11, OUTPUT);
Gpio lampPin(12, OUTPUT);

UpperSensors sensorHandler(floatLevelPin, acSensePin, false);
RsUpper<SerialWrapper, Crc8, 128> device(serial, DeviceType::Upper, damPin, lampPin, &sensorHandler);

void setup() 
{
	latch.reset();
	serial.init();

	led.set();
	delay(500);
	led.reset();
}

void loop() 
{
	if (serial.bytesAvaillable()) {
		size_t len = serial.bytesAvaillable();
		uint8_t buffer[64];

		len = min(sizeof(buffer), len);
		
		serial.read(buffer, len);
		device.update(buffer, len);
	}
}