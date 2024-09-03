/*!
@file
@brief ООП Обертка для GPIO
@author V-Nezlo (vlladimirka@gmail.com)
@date 06.10.2022
@version 1.0
*/

#ifndef INCLUDE_GPIOWRAPPER_HPP_
#define INCLUDE_GPIOWRAPPER_HPP_

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
	void reconfigure(int aType);

private:
	int number;
	bool checkPwmPinStatus();
};

#endif // INCLUDE_GPIOWRAPPER_HPP_