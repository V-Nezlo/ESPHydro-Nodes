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

Gpio latch(5, OUTPUT);
SerialWrapper serial(115200, latch);

Gpio led1(A4, OUTPUT);
Gpio led2(A2, OUTPUT);
Gpio led3(A0, OUTPUT);

Gpio waterLev1(1, INPUT);
Gpio waterLev2(2, INPUT);
Gpio waterLev3(3, INPUT);

Gpio pumpCurrentSensor(4, INPUT);
Gpio pumpPin(6, OUTPUT);

LowerSensors<1> sensorHandler(waterLev1, waterLev2, waterLev3, pumpCurrentSensor);
RsLower<SerialWrapper, Crc8, 128> device(serial, DeviceType::Lower, pumpPin, &sensorHandler);

uint32_t sensorLastUpdate;
static constexpr uint32_t kSensorsUpdateTimeMs{500};

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

	const auto currentTime = TimeWrapper::milliseconds();
	if (currentTime > sensorLastUpdate + kSensorsUpdateTimeMs) {
		sensorLastUpdate = currentTime;
		sensorHandler.process();
	}
}