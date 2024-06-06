#ifndef RSLOWER_HPP_
#define RSLOWER_HPP_

#include "AbstractSensorDataProvider.hpp"
#include "GpioWrapper.hpp"
#include "TimeWrapper.hpp"
#include <Lib/RsHandler.hpp>
#include <Types.hpp>

#include <stddef.h>
#include <stdint.h>

template<class Interface, typename Crc, size_t ParserSize>
class RsLower : public RS::RsHandler<Interface, Crc, ParserSize> {
	using BaseType = RS::RsHandler<Interface, Crc, ParserSize>;

public:
	RsLower(Interface &aInterface, uint8_t aNodeUID, Gpio &aPumpGpio, AbstractSensorDataProvider *aSensorHandler):
		BaseType{aInterface, aNodeUID},
		pump{aPumpGpio},
		pumpState{false},
		sensorHandler{aSensorHandler}
	{

	}

	uint8_t handleCommand(uint8_t aCommand, uint8_t aArgument) override
	{
		switch (aCommand) {
		case static_cast<uint8_t>(Commands::SetPumpState): {
			const bool newState = aArgument >= 1;
			pumpState = newState;
			pump.setState(newState);
			return 1;
			} break;
		
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

				LowerTelemetry telem = sensorHandler->getSensorData();
				telem.pumpState = pumpState;


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
	AbstractSensorDataProvider *sensorHandler;
};

#endif