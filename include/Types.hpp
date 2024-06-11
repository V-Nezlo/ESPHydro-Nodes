/*!
@file
@brief Типы
@author V-Nezlo (vlladimirka@gmail.com)
@date 04.05.2024
@version 1.0
*/

#ifndef INCLUDE_TYPES_HPP_
#define INCLUDE_TYPES_HPP_

#include <stdint.h>

enum DeviceType {
	Master = 0,
	Upper,
	Lower,
	AUX
};

enum LowerFlags : uint8_t {
	LowerPumpOverCurrentFlag = 0x01,
	LowerNoWaterFlag = 0x02,
	LowerTempSensorErrorFlag = 0x04,
	LowerPHSensorErrorFlag = 0x08,
	LowerPPMSensorErrorFlag = 0x10,
	LowerPumpLowCurrentFlag = 0x20
};

enum UpperFlags : uint8_t {
	UpperTopWaterLevelStuck = 0x01,
	UpperPowerError = 0x02
};

enum class PumpState {
    PumpOn = 0,
    PumpOff = 1,
};

enum class Requests {
	RequestTelemetry = 1
};

enum class Commands {
	SetPumpState = 1,
	SetLampState = 2,
	SetDamState = 3
};

struct LowerTelemetry {
	uint8_t pumpState;
	uint8_t waterLevelPerc;
	int8_t waterTemperature10;
	uint8_t waterPH10;
	uint16_t waterPPM;
	uint8_t deviceFlags;
} __attribute__((packed));

struct UpperTelemetry {
	uint8_t lampState;
	uint8_t swingLevelState;
	uint8_t damState;
	uint8_t deviceFlags;
} __attribute__((packed));

#endif