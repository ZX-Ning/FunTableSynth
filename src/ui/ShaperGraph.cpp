#include "ShaperGraph.h"
#include "../helpers.h"
#include <algorithm>
#include <cmath>
#include <vector>

namespace ZX {

static constexpr float FROM = -1.0, TO = 1.0;

void ShaperGraph::paint(juce::Graphics &g) {
    width = static_cast<float>(getWidth());
    height = static_cast<float>(getHeight());
    oX = width / 2;//origin: (oX, oY)
    oY = height / 2;
    length = std::min(width, height);
    float halfLength = length / 2;
    g.setColour(colorFromHex(0xB0BEC5));
    g.drawLine(oX - halfLength, oY, oX + halfLength, oY, 0.7f);//X-Axis
    g.drawLine(oX, oY - halfLength, oX, oY + halfLength, 0.7f);//Y-Axis
    g.drawRect(oX - halfLength, oY - halfLength, length, length, 2.0f);
    if (!table.empty()) {
        drawGraph(g);
    }
}

void ShaperGraph::drawGraph(juce::Graphics &g) {
    g.setColour(colorFromHex(0xE53935));
    float steps = std::floorf(TO - FROM) / (table.size() - 1);
    juce::Point<float> lastpoint;
    for (size_t i = 0; i < table.size(); ++i) {
        float x = FROM + i * steps;
        float y = table[i];
        y = inRange(y, -1.0f, 1.0f);
        float realX = oX + x * zoomRate * length / (TO - FROM);
        float realY = oY - y * zoomRate * length / 2;
        if (i == 0) {
            g.drawLine(realX, realY, realX, realY, 2.5f);
        }
        else {
            g.drawLine(juce::Line(
                               lastpoint,
                               juce::Point{realX, realY}),
                       2.5f);
        }
        lastpoint = {realX, realY};
    }
}

void ShaperGraph::resized() {
}

void ShaperGraph::setTable(std::vector<float> &&_table) {
    table = std::move(_table);
}

}// namespace ZX