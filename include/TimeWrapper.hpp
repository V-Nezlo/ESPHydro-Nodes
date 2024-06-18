/*!
@file
@brief ООП обертка для времени
@author V-Nezlo (vlladimirka@gmail.com)
@date 23.10.2022
@version 1.0
*/

#ifndef INCLUDE_TIMEWRAPPER_HPP_
#define INCLUDE_TIMEWRAPPER_HPP_

#include <Arduino.h>

class TimeWrapper {
public:
	static uint32_t milliseconds()
	{
		return millis();
	}

	static uint32_t seconds()
	{
		return millis() / 1000;
	}

};

#endif // INCLUDE_TIMEWRAPPER_HPP_
