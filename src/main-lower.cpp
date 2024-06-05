#include <Arduino.h>
#include <SoftwareSerial.h>
#include "SerialWrapper.hpp"
#include "GpioWrapper.hpp"
#include "RsTank.hpp"
#include "Crc8.hpp"

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

SensorHandler<1> sensorHandler(waterLev1, waterLev2, waterLev3, pumpCurrentSensor);
RsTank<SerialWrapper, Crc8, 128> device(serial, 1, pumpPin, &sensorHandler);

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