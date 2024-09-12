// /*!
// @file
// @brief Ловер
// @author V-Nezlo (vlladimirka@gmail.com)
// @date 11.05.2024
// @version 1.0
// */


#include "GpioWrapper.hpp"
#include "LowerSensors.hpp"
#include "SerialWrapper.hpp"
#include "RsLower.hpp"

#include <Arduino.h>
#include <Crc8.hpp>

Gpio latch(2, OUTPUT);
SerialWrapper serial(115200, latch);

Gpio EC_Sence(A0, INPUT);
Gpio EC_Gnd(A1, INPUT);
Gpio EC_Pow(A2, INPUT);

Gpio waterLev1(5, INPUT);
Gpio waterLev2(6, INPUT);
Gpio waterLev3(7, INPUT);

Gpio pumpPin(3, OUTPUT);
Gpio led(13, OUTPUT);
bool ledState = false;

LowerSensors<4> sensorHandler(waterLev1, waterLev2, waterLev3, EC_Sence, EC_Gnd, EC_Pow);
RsLower<SerialWrapper, Crc8, 128> device(serial, DeviceType::Lower, pumpPin, &sensorHandler);

uint32_t sensorLastUpdate;
static constexpr uint32_t kSensorsUpdateTimeMs{500};

void setup() 
{
	latch.reset();
	serial.init();
	sensorHandler.init();

	pumpPin.set();
	sensorHandler.checkPump();
	pumpPin.reset();
	
	led.set();
	delay(500);
	led.reset();
}

void loop() 
{
	if (serial.bytesAvaillable()) {
		size_t len = serial.bytesAvaillable();
		uint8_t buffer[64];
		serial.read(buffer, len);
		device.update(buffer, len);

		ledState = !ledState;
		led.setState(ledState);
	}

	const auto currentTime = TimeWrapper::milliseconds();
	if (currentTime > sensorLastUpdate + kSensorsUpdateTimeMs) {
		sensorLastUpdate = currentTime;
		sensorHandler.process();
	}
}
