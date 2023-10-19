#ifndef ZX_FUN_TABLE_SYNTH_UI_HELPERS_H
#define ZX_FUN_TABLE_SYNTH_UI_HELPERS_H


#include <math.h>
#include <cstdint>
#include <JuceHeader.h>
#include <limits>
#include <iostream>

namespace ZX{

constexpr double PI = 3.141592653589793;

static inline juce::Colour colorFromHex(uint32_t hex){
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

static inline juce::Font getJBMonoFont(bool isBold) {
    if (isBold) {
        auto f = juce::Font{
                juce::Typeface::createSystemTypefaceFor(
                        BinaryData::JetBrainsMonoNLBold_ttf,
                        BinaryData::JetBrainsMonoNLBold_ttfSize)};
        f.setBold(true);
        return f;
    }

    else {
        auto f = juce::Font{
                juce::Typeface::createSystemTypefaceFor(
                        BinaryData::JetBrainsMonoNLRegular_ttf,
                        BinaryData::JetBrainsMonoNLRegular_ttfSize)};
        f.setBold(false);
        return f;
    }
}

}

#endif //ZX_FUN_TABLE_SYNTH_UI_HELPERS_H
