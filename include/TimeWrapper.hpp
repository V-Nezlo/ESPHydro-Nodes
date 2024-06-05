//-----------------------------------------------------------------------
//  File        : TimeWrapper.hpp
//  Created     : 6.10.2022
//  Modified    : 23.10.2022
//  Author      : V-Nezlo (vlladimirka@gmail.com)
//  Description : ООП обертка для времени

#ifndef TIMEWRAPPER
#define TIMEWRAPPER

#include <Arduino.h>

class TimeWrapper {
public:
    static uint32_t milliseconds()
    {
        return millis();
    }

    static uint32_t seconds()
    {
        return millis() / 1000;
    }

};

#endif
