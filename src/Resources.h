//
// Created by Lemon Ning on 2023/10/16.
//

#ifndef ZX_FUN_SHAPER_RESOURCES_H_
#define ZX_FUN_SHAPER_RESOURCES_H_

#include "JuceHeader.h"

namespace ZX {

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


#endif //ZX_FUN_SHAPER_RESOURCES_H_
