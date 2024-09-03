/*!
@file
@brief ООП Обертка для GPIO
@author V-Nezlo (vlladimirka@gmail.com)
@date 06.10.2022
@version 1.0
*/

#include "GpioWrapper.hpp"

constexpr uint8_t Gpio::kPwmPins[];

Gpio::Gpio(int aNum, int aType):
	number{aNum}
{
	pinMode(number, aType);
}

Gpio::~Gpio()
{
	// При удалении обьекта поставим пин в выход и низкое положение
	pinMode(number, OUTPUT);
	digitalWrite(number, LOW);
}

void Gpio::set()
{
	digitalWrite(number, HIGH);
}

void Gpio::reset()
{
	digitalWrite(number, LOW);
}

void Gpio::setState(bool aState)
{
	if (aState) {
		digitalWrite(number, HIGH);
	} else {
		digitalWrite(number, LOW);
	}
}

bool Gpio::digitalRead()
{
	return static_cast<bool>(::digitalRead(number));
}

int Gpio::analogRead()
{
	return ::analogRead(number);
}

void Gpio::analogWrite(int aValue)
{
	// Ремарка - внутри analogWrite уже есть проверка на присутствие таймера на ноге
	// Почему сделано именно так - там стоит кривое условие, если val < 128 то это 0, если больше - 255
	// Крайне неудобно это учитывать в коде, поэтому я заменяю проверку этого условия
	if (checkPwmPinStatus()) {
		return ::analogWrite(number, aValue);
	} else {
		return ::digitalWrite(number, static_cast<uint8_t>(aValue));
	}
}

void Gpio::reconfigure(int aType)
{
	pinMode(number, aType);
}

bool Gpio::checkPwmPinStatus()
{
	for (auto pos : kPwmPins) {
		if (number == pos) {
			return true;
		}
	}

	return false;
}
