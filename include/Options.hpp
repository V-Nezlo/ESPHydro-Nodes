/*!
@file
@brief Разные параметры устройств
@author V-Nezlo (vlladimirka@gmail.com)
@date 20.07.2024
@version 1.0
*/

#ifndef INCLUDE_OPTIONS_HPP_
#define INCLUDE_OPTIONS_HPP_

namespace Options {

namespace Lower {
    static constexpr float kMaxCurrentToOvercurrent_mA{1000.f};
    static constexpr float kMinCurrentToNoLoad{50.f};
    static constexpr uint8_t kMinWaterLevelForError{10};
} // namespace Lower

} // namespace Options

#endif // INCLUDE_OPTIONS_HPP_