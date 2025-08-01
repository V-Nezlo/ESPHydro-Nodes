/*!
@file
@brief Обертка для сериала
@author V-Nezlo (vlladimirka@gmail.com)
@date 06.04.2024
@version 1.0
*/

#ifndef INCLUDE_SERIALWRAPPER_HPP
#define INCLUDE_SERIALWRAPPER_HPP

#include "GpioWrapper.hpp"

#include <Arduino.h>
#include <string.h>
#include <stdint.h>


class SerialWrapper {
public:
	SerialWrapper(uint32_t aSpeed, Gpio &aLatch, Gpio &aIndic): 
	speed{aSpeed}, 
	latch{aLatch},
	indic{aIndic}
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

	size_t write(const void *aData, size_t aLength)
	{
		latch.set();
		indic.set();
		delay(20);
		auto len = Serial.write(static_cast<const char *>(aData), aLength);
		delay(20);
		latch.reset();
		indic.reset();
		return len;
	}

private:
	uint32_t speed;
	Gpio &latch;
	Gpio &indic;
};

#endif // INCLUDE_SERIALWRAPPER_HPP
