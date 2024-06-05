#ifndef INCLIDE_ABSTRACTSENSORDATAPROVIDER_HPP_
#define INCLIDE_ABSTRACTSENSORDATAPROVIDER_HPP_

#include <stdint.h>
#include "Types.hpp"

class AbstractSensorDataProvider {
public:
    virtual LowerTelemetry &getSensorData() = 0;
};

#endif