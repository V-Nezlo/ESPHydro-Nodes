/*!
@file
@brief RS класс аппера
@author V-Nezlo (vlladimirka@gmail.com)
@date 03.06.2024
@version 1.0
*/

#ifndef INCLUDE_RSUPPER_HPP_
#define INCLUDE_RSUPPER_HPP_

#include "AbstractDataProvider.hpp"
#include "GpioWrapper.hpp"
#include "TimeWrapper.hpp"
#include "Types.hpp"

#include <UtilitaryRS/RsHandler.hpp>
#include <stddef.h>
#include <stdint.h>

template<class Interface, typename Crc, size_t ParserSize>
class RsUpper : public RS::RsHandler<Interface, Crc, ParserSize> {
	using BaseType = RS::RsHandler<Interface, Crc, ParserSize>;

public:
	RsUpper(Interface &aInterface, uint8_t aNodeUID, Gpio &aDamGpio, Gpio &aLampPin, AbstractUpperDataProvider *aSensorHandler):
		BaseType{aInterface, aNodeUID},
		dam{aDamGpio},
		damState{false},
		lamp{aLampPin},
		lampState{false},
		sensorHandler{aSensorHandler}
	{

	}

	uint8_t handleCommand(uint8_t aCommand, uint8_t aArgument) override
	{
		const auto command = static_cast<Commands>(aCommand);
		const bool value = aArgument >= 1 ? true : false;

		switch (command) {
			case Commands::SetDamState:
				damState = value;
				dam.setState(value);
				return 1;
				break;
		
			case Commands::SetLampState:
				lampState = value;
				lamp.setState(value);
				return 1;
				break;

			default:
				return 0;
				break;
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

				UpperTelemetry telem = sensorHandler->getSensorData();
				telem.damState = damState;
				telem.lampState = lampState;

				return BaseType::sendAnswer(aTransmitUID, aRequest, aRequestedDataSize, &telem, sizeof(telem));
			} break;
			default:
				return false;
				break;
		}
	}
private:
	Gpio &dam;
	bool damState;

	Gpio &lamp;
	bool lampState;

	AbstractUpperDataProvider *sensorHandler;
};

#endif // INCLUDE_RSUPPER_HPP_