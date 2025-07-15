/*!
@file
@brief Сенсор хандлер для аппера
@author V-Nezlo (vlladimirka@gmail.com)
@date 11.06.2024
@version 1.0
*/

#ifndef INCLUDE_UPPERSENSORS_HPP_
#define INCLUDE_UPPERSENSORS_HPP_

#include "TimeWrapper.hpp"
#include "GpioWrapper.hpp"
#include "AbstractDataProvider.hpp"

class UpperSensors : public AbstractUpperDataProvider {
public:
	UpperSensors(Gpio &aFloatLevel, Gpio &aACSense, bool aLevInversion) :
		floatLev{aFloatLevel},
		acSense{aACSense},
		floatLevState{false},
		levInversion{aLevInversion},
		lastUpdateTime{0},
		levelStuckTimer{0}
	{
	}

	UpperTelemetry getSensorData() override
	{
		UpperTelemetry telem;
		telem.deviceFlags = 0;

		const uint16_t raw = floatLev.analogRead();
		const float voltage = raw * 5.0f / 1023.0f;

		if (voltage > 4.f) {
			telem.deviceFlags |= UpperFlags::UpperTopWaterLevelError;
			telem.swingLevelState = false;
		} else if (voltage < 1.8f) {
			telem.swingLevelState = true;
		} else {
			telem.swingLevelState = false;
		}

		// if (!acSense.digitalRead()) {
		// 	telem.deviceFlags |= UpperFlags::UpperPowerError;
		// }

		return telem;
	}

private:
	Gpio &floatLev;
	Gpio &acSense;
	bool floatLevState;
	const bool levInversion;

	uint32_t lastUpdateTime; // mS
	int32_t levelStuckTimer; // mS
};

#endif // INCLUDE_UPPERSENSORS_HPP_