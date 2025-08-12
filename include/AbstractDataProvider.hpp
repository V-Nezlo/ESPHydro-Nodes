/*!
@file
@brief Абстрактные классы сенсоров
@author V-Nezlo (vlladimirka@gmail.com)
@date 20.05.2024
@version 1.0
*/

#ifndef INCLIDE_ABSTRACTDATAPROVIDER_HPP_
#define INCLIDE_ABSTRACTDATAPROVIDER_HPP_

#include "Types.hpp"
#include <stdint.h>

class AbstractLowerDataProvider {
public:
	virtual LowerTelemetry getSensorData() = 0;
	virtual void setTelemPumpState(PumpState aState) = 0;
};

class AbstractUpperDataProvider {
public:
	virtual UpperTelemetry getSensorData() = 0;
};

#endif