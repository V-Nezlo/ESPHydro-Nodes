//-----------------------------------------------------------------------
//  File        : GpioWrapper.hpp
//  Created     : 6.10.2022
//  Modified    : 23.10.2022
//  Author      : V-Nezlo (vlladimirka@gmail.com)
//  Description : ООП обертка для gpio

#ifndef GPIOWRAPPER
#define GPIOWRAPPER

#include <Arduino.h>

class Gpio {
	static constexpr uint8_t kPwmPins[] = {3, 5, 6, 9, 10, 11};
public:
	Gpio(int aNum, int aType);
	// Чтобы не облажаться удалю ненужные конструкторы
	Gpio(const Gpio &) = delete;
	Gpio &operator=(const Gpio &) = delete;
	virtual ~Gpio();

	void set();
	void reset();
	void setState(bool aState);
	bool digitalRead();
	int analogRead();
	void analogWrite(int aValue);

private:
    int number;
	bool checkPwmPinStatus();
};

#endif