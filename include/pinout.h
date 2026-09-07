#ifndef __PINOUT_H__
#define __PINOUT_H__

#include <Arduino.h>


namespace Pins{
    constexpr uint8_t Encoder_A =  5;
    constexpr uint8_t Encoder_B =  6;
    constexpr uint8_t LED       =  4;
    constexpr uint8_t DISPLAY_CLK =  1;
    constexpr uint8_t DISPLAY_DIO =  0;
}

#endif // __PINOUT_H__