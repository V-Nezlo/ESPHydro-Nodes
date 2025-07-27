/*!
@file
@brief Фильтр для ppm
@author V-Nezlo (vlladimirka@gmail.com)
@date 16.09.2024
@version 1.0
*/

#ifndef INCLUDE_PPMFILTER_HPP_
#define INCLUDE_PPMFILTER_HPP_

class PpmFilter {
public:
    static float apply(float aNewVal)
    {
        static float prevValue = 0;
        prevValue += (aNewVal - prevValue) * kCoeff;
        return prevValue;
    }

    static bool isValid(uint16_t aPpm)
    {
        static constexpr uint16_t kMinValue = 10;
        static constexpr uint16_t kMaxValue = 5000;

        return aPpm > kMinValue && aPpm < kMaxValue;
    }

private:
    static constexpr float kCoeff{0.5f};
};

#endif // INCLUDE_PPMFILTER_HPP_