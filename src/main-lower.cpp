/*!
@file
@brief Ловер
@author V-Nezlo (vlladimirka@gmail.com)
@date 11.05.2024
@version 1.0
*/

#include "GpioWrapper.hpp"
#include "LowerSensors.hpp"
#include "SerialWrapper.hpp"
#include "RsLower.hpp"

#include <Arduino.h>
#include <SoftwareSerial.h>
#include <UtilitaryRS/Crc8.hpp>

Gpio latch(2, OUTPUT);
SerialWrapper serial(115200, latch);

Gpio waterLev1(5, INPUT);
Gpio waterLev2(6, INPUT);
Gpio waterLev3(7, INPUT);

Gpio pumpPin(3, OUTPUT);

LowerSensors<1> sensorHandler(waterLev1, waterLev2, waterLev3);
RsLower<SerialWrapper, Crc8, 128> device(serial, DeviceType::Lower, pumpPin, &sensorHandler);

uint32_t sensorLastUpdate;
static constexpr uint32_t kSensorsUpdateTimeMs{500};

void setup() 
{
	latch.reset();
	serial.init();
	sensorHandler.init();
}

void loop() 
{
	if (serial.bytesAvaillable()) {
		size_t len = serial.bytesAvaillable();
		uint8_t buffer[64];
		serial.read(buffer, len);
		device.update(buffer, len);
	}

	const auto currentTime = TimeWrapper::milliseconds();
	if (currentTime > sensorLastUpdate + kSensorsUpdateTimeMs) {
		sensorLastUpdate = currentTime;
		sensorHandler.process();
	}
}