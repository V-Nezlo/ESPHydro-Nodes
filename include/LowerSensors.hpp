/*!
@file
@brief Обработчик сенсоров ловера
@author V-Nezlo (vlladimirka@gmail.com)
@date 20.05.2024
@version 1.0
*/

#ifndef INCLUDE_LOWERHANDLER_HPP_
#define INCLUDE_LOWERHANDLER_HPP_

#include "PpmFilter.hpp"
#include "LowerFlagStorage.hpp"
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

		inversion{true},
		curSensor{},
		expRunFilterValue{0.f},

		lastCheckTime{0},
		updateTime{200},
		tempSensor{},
		data{},
		pumpState{PumpState::PumpOff},
		pumpCheckTimestamp{0}
	{
		aEC_Gnd.reset();
		aEC_Pow.reset();

		floatLev1.set();
		floatLev2.set();
		floatLev3.set();
	}

	bool init()
	{
		bool result = curSensor.begin();
		tempSensor.setResolution(12);

		return result;
	}

	void setTelemPumpState(PumpState aState) override
	{
		pumpState = aState;

		// Отложим запуск проверки насоса чтобы фильтр успел накопиться
		pumpCheckTimestamp = TimeWrapper::milliseconds();
	}

	void process()
	{
		const uint32_t currentTime = TimeWrapper::milliseconds();

		if (currentTime - lastCheckTime >= updateTime) {
			lastCheckTime = currentTime;

			// Проверим что температурный сенсор отвечает
			const bool tempReadResult = tempSensor.readTemp();
			// Проверяем состояние насоса постоянно
			const auto pumpCheckerState = checkPump();

			if (currentTime - pumpCheckTimestamp >= 1000) {
				pumpCheckTimestamp = currentTime;

				if (pumpCheckerState == PumpChecker::NoCurrent) {
					data.deviceFlags |= LowerFlags::LowerPumpLowCurrentFlag;
				} else if (pumpCheckerState == PumpChecker::Overcurrent) {
					data.deviceFlags |= LowerFlags::LowerPumpOverCurrentFlag;
				} else {
					data.deviceFlags &= ~LowerFlags::LowerPumpOverCurrentFlag;
					data.deviceFlags &= ~LowerFlags::LowerPumpLowCurrentFlag;
				}
			}

			// Запрос температуры и установка-сброс флагов ошибки
			if (tempReadResult) {
				const float temp = tempSensor.getTemp();
				data.waterTemperature10 = temp * 10;
				const uint16_t ppm = getPPM(temp);

				if (PpmFilter::isValid(ppm)) {
					data.waterPPM = ppm;
					data.deviceFlags &= ~LowerFlags::LowerPPMSensorErrorFlag;
				} else {
					data.waterPPM = 0;
					data.deviceFlags |= LowerFlags::LowerPPMSensorErrorFlag;
				}
				data.deviceFlags &= ~LowerFlags::LowerTempSensorErrorFlag;
			} else {
				data.deviceFlags |= LowerFlags::LowerTempSensorErrorFlag;
				data.waterTemperature10 = 0;
			}

			data.waterLevelPerc = getWaterLevel();
			if (data.waterLevelPerc > Options::Lower::kMinWaterLevelForError) {
				data.deviceFlags &= ~LowerFlags::LowerNoWaterFlag;
			} else {
				data.deviceFlags |= LowerFlags::LowerNoWaterFlag;
			}

			uint8_t ph10 = getPH();
			if (data.waterPH10 != 0) {
				data.waterPH10 = ph10;
				data.deviceFlags &= ~LowerFlags::LowerPHSensorErrorFlag;
			} else {
				data.waterPH10 = 0;
				data.deviceFlags |= LowerFlags::LowerPHSensorErrorFlag;
			}

			// Запрос измерения температуры для следующего чтения
			tempSensor.requestTemp();
		}
	}

	LowerTelemetry getSensorData() override
	{
		return data;
	}

	PumpChecker checkPump()
	{
		float current = curSensor.getCurrent_mA();
		expRunFilterValue += (current - expRunFilterValue) * 0.2f;
		current = expRunFilterValue;

		if (!FlagStorage::instance().pumpState) {
			return PumpChecker::Disabled;
		}

		if (current >= Options::Lower::kMaxCurrentToOvercurrent_mA) {
			return PumpChecker::Overcurrent;
		}

		if (current <= Options::Lower::kMinCurrentToNoLoad) {
			return PumpChecker::NoCurrent;
		}

		return PumpChecker::Normal;
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
	float expRunFilterValue;

	uint32_t lastCheckTime;
	const uint32_t updateTime;
	MicroDS18B20<DSPin> tempSensor;

	LowerTelemetry data;

	PumpState pumpState;
	uint32_t pumpCheckTimestamp;

	uint8_t getWaterLevel()
	{
		const bool w1 = inversion ? !floatLev1.digitalRead() : floatLev1.digitalRead();
		const bool w2 = inversion ? !floatLev2.digitalRead() : floatLev2.digitalRead();
		const bool w3 = inversion ? !floatLev3.digitalRead() : floatLev3.digitalRead();

		if (!w1 && !w2 && !w3) return 0;
		if (w1 && !w2 && !w3)  return 33;
		if (w1 && w2 && !w3)   return 66;
		if (w1 && w2 && w3)    return 100;

		// Если сработал верхний или средний без нижнего — ошибка
		data.deviceFlags |= LowerFlags::LowerWaterLevelError;
		return 0;
	}

	uint16_t getPPM(float aCurrentTemp)
	{
		ecPow.set();
		ecSence.analogRead();
		ecSence.analogRead();
		ecSence.analogRead();
		const float vRaw = ecSence.analogRead();
		ecPow.reset();

		const float vDrop =  (kVin * vRaw) / 1024.f;
		const float rC = ((vDrop * kPowResistorValue) / (kVin - vDrop) + 0.001f) - kIntResistorValue;
		const float ec = 1000 / ((rC * kCellConstantK) + 0.001f);

		const float ec25 = ec / (1 + kTemperatureCoef * (aCurrentTemp - 25.f));
		return static_cast<uint16_t>(PpmFilter::apply(static_cast<float>(ec25 * kPPMconversion * 1000)));
	}

	uint8_t getPH()
	{
		return 0;
	}
};

#endif // INCLUDE_LOWERHANDLER_HPP_