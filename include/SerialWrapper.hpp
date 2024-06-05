#ifndef SERIALWRAPPER_HPP
#define SERIALWRAPPER_HPP

#include <string.h>
#include <stdint.h>
#include "Arduino.h"
#include "GpioWrapper.hpp"

class SerialWrapper {
public:
	SerialWrapper(uint32_t aSpeed, Gpio &aLatch) : speed{aSpeed}, latch{aLatch}
	{}

	void init()
	{
		return Serial.begin(speed);
	}

	size_t bytesAvaillable()
	{
		return Serial.available();
	}

	size_t read(uint8_t *aBuffer, size_t aLength)
	{
		return Serial.readBytes(aBuffer, aLength);
	}

	size_t write(const uint8_t *aData, size_t aLength)
	{
		latch.set();
		auto len = Serial.write(aData, aLength);
		latch.reset();
		return len;
	}

private:
	uint32_t speed;
	Gpio &latch;
};

#endif // SERIALWRAPPER_HPP
