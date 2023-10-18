#ifndef ZX_FUN_SHAPER_UI_HELPERS_H
#define ZX_FUN_SHAPER_UI_HELPERS_H

#include <cstdint>
#include <JuceHeader.h>
#include <limits>
#include <iostream>

namespace ZX{

static juce::Colour colorFromHex(uint32_t hex){
    return juce::Colour::fromRGB(static_cast<uint8_t>(hex >> 16),
                                 static_cast<uint8_t>((hex & 0x00FF00) >> 8),
                                 static_cast<uint8_t>(hex & 0x0000FF));
}

template<typename T>
static inline T inRange(T num,T bottom, T top){
    T epsilon = std::numeric_limits<T>::epsilon();
    if (num - top > epsilon) {
        return top;
    }
    if (bottom - num > epsilon) {
        return bottom;
    }
    return num;
}

}

#endif //ZX_FUN_SHAPER_UI_HELPERS_H
