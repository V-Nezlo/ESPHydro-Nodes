/*!
@file
@brief RS класс ловера
@author V-Nezlo (vlladimirka@gmail.com)
@date 20.05.2024
@version 1.0
*/

#ifndef INCLUDE_RSLOWER_HPP_
#define INCLUDE_RSLOWER_HPP_

#include "LowerFlagStorage.hpp"
#include "AbstractDataProvider.hpp"
#include "GpioWrapper.hpp"
#include "TimeWrapper.hpp"
#include "Types.hpp"

#include <RsHandler.hpp>
#include <stddef.h>
#include <stdint.h>

template<class Interface, typename Crc, size_t ParserSize>
class RsLower : public RS::RsHandler<Interface, Crc, ParserSize> {
	using BaseType = RS::RsHandler<Interface, Crc, ParserSize>;

public:
	RsLower(Interface &aInterface, uint8_t aNodeUID, Gpio &aPumpGpio, AbstractLowerDataProvider *aSensorHandler):
		BaseType{aInterface, aNodeUID},
		pump{aPumpGpio},
		pumpState{false},
		sensorHandler{aSensorHandler},
		calibrationTimeout{0},
		calibrate{false}
	{
	}

	uint8_t handleCommand(uint8_t aCommand, uint8_t aArgument) override
	{
		Commands command = static_cast<Commands>(aCommand);

		switch (command) {
			case Commands::SetPumpState: {
				const bool newState = aArgument >= 1;
				pumpState = newState;
				pump.setState(newState);

				FlagStorage::instance().pumpState = newState;
				sensorHandler->setTelemPumpState(newState ? PumpState::PumpOn : PumpState::PumpOff);
				return 1;
				};

			case Commands::CalibECSensor: {
				return 1;
			} break;

			default:
				return 0;
			}
	}

	void handleAck(uint8_t aTranceiverUID, uint8_t aReturnCode) override
	{
		// Пока не обрабатывает Ack от мастера
		return;
	}

	uint8_t handleAnswer(uint8_t aTranceiverUID, uint8_t aRequest, const uint8_t *aData, uint8_t aLength) override
	{
		// Слейв не делает запросов
		return 0;
	}

	bool processRequest(uint8_t aTransmitUID, uint8_t aRequest, uint8_t aRequestedDataSize) override
	{
		switch(aRequest) {
			case static_cast<uint8_t>(Requests::RequestTelemetry): {
				// Если длина запроса не совпадает - вернется ACK с кодом 0

				const LowerTelemetry telem = sensorHandler->getSensorData();

				return BaseType::sendAnswer(aTransmitUID, aRequest, aRequestedDataSize, &telem, sizeof(telem));
			} break;
			default:
				return false;
				break;
		}
	}
private:
	Gpio &pump;
	bool pumpState;
	AbstractLowerDataProvider *sensorHandler;

	uint32_t calibrationTimeout;
	bool calibrate;
};

#endif // INCLUDE_RSLOWER_HPP_