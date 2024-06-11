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

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <UtilitaryRS/Crc8.hpp>

Gpio latch(5, OUTPUT);
SerialWrapper serial(115200, latch);

Gpio acSensePin(4, INPUT);
Gpio floatLevelPin(5, INPUT);
Gpio damPin(6, OUTPUT);
Gpio lampPin(7, OUTPUT);

UpperSensors sensorHandler(floatLevelPin, acSensePin, false);
RsUpper<SerialWrapper, Crc8, 128> device(serial, DeviceType::Upper, damPin, lampPin, &sensorHandler);

void setup() 
{
	latch.reset();
	serial.init();
}

void loop() 
{
	if (serial.bytesAvaillable()) {
		size_t len = serial.bytesAvaillable();
		uint8_t buffer[64];
		serial.read(buffer, len);
		device.update(buffer, len);
	}

}