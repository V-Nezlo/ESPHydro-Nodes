#ifndef INCLUDE_SENSORHANDLER_HPP_
#define INCLUDE_SENSORHANDLER_HPP_

#include "AbstractSensorDataProvider.hpp"
#include <GpioWrapper.hpp>
#include <TimeWrapper.hpp>
#include <Types.hpp>
#include <microDS18B20.h>
#include <stdint.h>

template<uint8_t DSPin>
class SensorHandler : public AbstractSensorDataProvider {
public:
    SensorHandler(Gpio &aFloatLev1, Gpio &aFloatLev2, Gpio &aFloatLev3, Gpio &aPumpCurrentSensorPin):
        floatLev1{aFloatLev1},
        floatLev2{aFloatLev2},
        floatLev3{aFloatLev3},
        inversion{false},
        currentSensor{aPumpCurrentSensorPin},

        lastCheckTime{0},
        updateTime{200},
        tempSensor{}
    {

    }

    void process()
    {
        uint32_t currentTime = TimeWrapper::milliseconds();

        if (lastCheckTime + updateTime < currentTime) {
            lastCheckTime = currentTime;

            // Проверим что температурный сенсор отвечает
            const bool tempReadResult = tempSensor.readTemp();

            // Запрос температуры и установка-сброс флагов ошибки
            if (tempReadResult) {
                data.waterTemperature10 = tempSensor.getTemp() * 10;
                data.deviceFlags &= ~static_cast<uint8_t>(LowerFlags::LowerTempSensorErrorFlag);
            } else {
                data.deviceFlags |= static_cast<uint8_t>(LowerFlags::LowerTempSensorErrorFlag);
                data.waterTemperature10 = 0;
            }

            const int adcData = currentSensor.analogRead();
            const uint16_t current = static_cast<uint16_t>(static_cast<float>(adcData) * static_cast<float>(5 / 1024));
            
            data.waterLevelPerc = getWaterLevel();

            data.waterPH10 = 0;
            data.waterPPM = 0;

            // Запрос измерения температуры для следующего чтения
            tempSensor.requestTemp();
        }
    }

    LowerTelemetry &getSensorData() override
    {
        return data;
    }

private:
    Gpio &floatLev1;
    Gpio &floatLev2;
    Gpio &floatLev3;
    bool inversion;

    Gpio &currentSensor;

    uint32_t lastCheckTime;
    const uint32_t updateTime;
    MicroDS18B20<DSPin> tempSensor;

    LowerTelemetry data;

    uint8_t getWaterLevel()
    {
        uint8_t procent{0};

        const bool water1State = floatLev1.digitalRead(); // Самый низкий датчик, он есть всегда
        const bool water2State = floatLev2.digitalRead(); // Датчик повыше
        const bool water3State = floatLev3.digitalRead(); // Высокий датчик ваще жесть

        if (inversion) {
            water1State = !water1State;
            water2State = !water2State;
            water3State = !water3State;
        }

        static constexpr uint8_t kLevelFive3Procent{80};
	    static constexpr uint8_t kLevelFive2Procent{60};
	    static constexpr uint8_t kLevelFive1Procent{40};
	    static constexpr uint8_t kLevelFive0Procent{20};

        if (water3State && water2State && water1State) {
            procent = 100;
        } else if (water2State && water1State) {
            procent = 75;
        } else if (water1State) {
            procent = 50;
        } else {
            procent = 25;
        }

        return procent;
    }
};

#endif