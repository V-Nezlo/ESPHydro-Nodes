#ifndef INCLUDE_TYPES_HPP_
#define INCLUDE_TYPES_HPP_

#include <stdint.h>

enum LowerFlags : uint8_t {
	LowerPumpOverCurrentFlag = 0x01,
	LowerNoWaterFlag = 0x02,
	LowerTempSensorErrorFlag = 0x04,
	LowerPHSensorErrorFlag = 0x08,
	LowerPPMSensorErrorFlag = 0x10,
	LowerPumpLowCurrentFlag = 0x20
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

#endif