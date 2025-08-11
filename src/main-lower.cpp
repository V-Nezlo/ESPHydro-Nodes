// /*!
// @file
// @brief Ловер
// @author V-Nezlo (vlladimirka@gmail.com)
// @date 11.05.2024
// @version 1.0
// */

#include "LowerFlagStorage.hpp"
#include "GpioWrapper.hpp"
#include "LowerSensors.hpp"
#include "SerialWrapper.hpp"
#include "RsLower.hpp"

#include <Arduino.h>
#include <avr/wdt.h>
#include <Crc8.hpp>

Gpio latch(2, OUTPUT);
Gpio led(5, OUTPUT);
Gpio onBoardLed(13, OUTPUT);
SerialWrapper serial(115200, latch, led);

Gpio EC_Sence(A1, INPUT);
Gpio EC_Gnd(A0, OUTPUT);
Gpio EC_Pow(A2, OUTPUT);

Gpio waterLev1(11, INPUT);
Gpio waterLev2(10, INPUT);
Gpio waterLev3(9, INPUT);

Gpio pumpPin(12, OUTPUT);

LowerSensors<7> sensorHandler(waterLev1, waterLev2, waterLev3, EC_Sence, EC_Gnd, EC_Pow);
RsLower<SerialWrapper, Crc8, 128> device(serial, DeviceType::Lower, pumpPin, &sensorHandler);

uint32_t sensorLastUpdate{0};
static constexpr uint32_t kSensorsUpdateTimeMs{500};

void setup() 
{
	latch.reset();
	serial.init();
	sensorHandler.init();

	pumpPin.set();
	delay(100);
	sensorHandler.checkPump();
	pumpPin.reset();

	wdt_enable(WDTO_4S);
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

	const auto currentTime = TimeWrapper::milliseconds();
	if (currentTime - sensorLastUpdate >= kSensorsUpdateTimeMs) {
		sensorLastUpdate = currentTime;
		sensorHandler.process();

		static bool ledState = false;
		ledState = !ledState;
		onBoardLed.setState(ledState);
	}
}