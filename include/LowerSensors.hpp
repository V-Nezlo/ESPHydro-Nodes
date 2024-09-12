/*!
@file
@brief Обработчик сенсоров ловера
@author V-Nezlo (vlladimirka@gmail.com)
@date 20.05.2024
@version 1.0
*/

#ifndef INCLUDE_LOWERHANDLER_HPP_
#define INCLUDE_LOWERHANDLER_HPP_

#include "AbstractDataProvider.hpp"
#include "GpioWrapper.hpp"
#include "Options.hpp"
#include "TimeWrapper.hpp"
#include "Types.hpp"
#include "Adafruit_INA219.h"
#include <microDS18B20.h>
#include <stdint.h>

template<uint8_t DSPin>
class LowerSensors : public AbstractLowerDataProvider {

	uint16_t kPowResistorValue = 1000; // External resistor
	uint16_t kIntResistorValue = 25;   // Resistance of analog pin

	const float kPPMconversion = 0.7f;
	const float kTemperatureCoef = 0.019f;
	const float kCellConstantK = 2.88f;
	const float kVin = 5.f;

public:
	LowerSensors(Gpio &aFloatLev1, Gpio &aFloatLev2, Gpio &aFloatLev3, Gpio &aEC_Sence, Gpio &aEC_Gnd, Gpio &aEC_Pow):
		floatLev1{aFloatLev1},
		floatLev2{aFloatLev2},
		floatLev3{aFloatLev3},

		ecSence{aEC_Sence},
		ecGnd{aEC_Gnd},
		ecPow{aEC_Pow},

		inversion{false},
		curSensor{},

		lastCheckTime{0},
		updateTime{200},
		tempSensor{}
	{
		aEC_Gnd.reset();
		aEC_Pow.reset();
	}

	void init()
	{
		curSensor.begin();
		curSensor.setCalibration_16V_400mA();
		tempSensor.setResolution(12);
	}

	void process()
	{
		uint32_t currentTime = TimeWrapper::milliseconds();

		if (lastCheckTime + updateTime < currentTime) {
			lastCheckTime = currentTime;

			// Проверим что температурный сенсор отвечает
			const bool tempReadResult = tempSensor.readTemp();

			checkPump();

			// Запрос температуры и установка-сброс флагов ошибки
			if (tempReadResult) {
				const float temp = tempSensor.getTemp();
				data.waterTemperature10 = temp * 10;
				data.waterPPM = getPPM(temp);
				data.deviceFlags &= ~static_cast<uint8_t>(LowerFlags::LowerTempSensorErrorFlag);
			} else {
				data.deviceFlags |= static_cast<uint8_t>(LowerFlags::LowerTempSensorErrorFlag);
				data.waterTemperature10 = 0;
			}

			data.waterLevelPerc = getWaterLevel();
			data.waterPH10 = 0;

			// Запрос измерения температуры для следующего чтения
			tempSensor.requestTemp();
		}
	}

	LowerTelemetry getSensorData() override
	{
		return data;
	}

	bool checkPump()
	{
		const float current = curSensor.getCurrent_mA();

		if (current >= Options::Lower::kMaxCurrentToOvercurrent_mA) {
			data.deviceFlags |= static_cast<uint8_t>(LowerFlags::LowerPumpOverCurrentFlag);
			return false;
		} else if (current <= Options::Lower::kMinCurrentToNoLoad) {
			data.deviceFlags |= static_cast<uint8_t>(LowerFlags::LowerPumpLowCurrentFlag);
			return false;
		} else {
			data.deviceFlags &= ~static_cast<uint8_t>(LowerFlags::LowerPumpOverCurrentFlag);
			data.deviceFlags &= ~static_cast<uint8_t>(LowerFlags::LowerPumpLowCurrentFlag);
			return true;
		}
	}

private:
	Gpio &floatLev1;
	Gpio &floatLev2;
	Gpio &floatLev3;

	Gpio &ecSence;
	Gpio &ecGnd;
	Gpio &ecPow;

	bool inversion;

	Adafruit_INA219 curSensor;

	uint32_t lastCheckTime;
	const uint32_t updateTime;
	MicroDS18B20<DSPin> tempSensor;

	LowerTelemetry data;

	uint8_t getWaterLevel()
	{
		uint8_t procent{0};

		const bool water1State = inversion ? !floatLev1.digitalRead() : floatLev1.digitalRead(); // Самый низкий датчик, он есть всегда
		const bool water2State = inversion ? !floatLev2.digitalRead() : floatLev1.digitalRead(); // Датчик повыше
		const bool water3State = inversion ? !floatLev3.digitalRead() : floatLev1.digitalRead(); // Высокий датчик ваще жесть

		static constexpr uint8_t kLevelFive3Procent{80};
		static constexpr uint8_t kLevelFive2Procent{60};
		static constexpr uint8_t kLevelFive1Procent{40};
		static constexpr uint8_t kLevelFive0Procent{20};

		if (water3State && water2State && water1State) {
			procent = kLevelFive3Procent;
		} else if (water2State && water1State) {
			procent = kLevelFive2Procent;
		} else if (water1State) {
			procent = kLevelFive1Procent;
		} else {
			procent = kLevelFive0Procent;
		}

		return procent;
	}

	uint16_t getPPM(float aCurrentTemp)
	{
		ecPow.set();
		float vRaw = ecSence.analogRead();
		vRaw = ecSence.analogRead();
		ecPow.reset();

		float vDrop =  (kVin * vRaw) / 1024.f;
		float rC = (vDrop * kPowResistorValue) / (kVin - vDrop);
		rC = rC - kIntResistorValue;
		float ec = 1000 / (rC * kCellConstantK);

		float ec25 = ec / (1 + kTemperatureCoef * (aCurrentTemp - 25.f));
		uint16_t ppm = static_cast<uint16_t>(ec25 * kPPMconversion * 1000);
		return ppm;
	}

};

#endif // INCLUDE_LOWERHANDLER_HPP_