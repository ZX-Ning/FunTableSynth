#include "ShaperGraph.h"
#include "../helpers.h"
#include <cmath>
#include <vector>

namespace ZX {

static constexpr float FROM = float(-M_PI), TO = float(M_PI);

ShaperGraph::ShaperGraph(std::vector<float> *_table)
        : table(_table) {}

// expand X to 2X, so the ratio becomes (pi : 2)
void ShaperGraph::paint(juce::Graphics &g) {
    width = static_cast<float>(getWidth());
    height = static_cast<float>(getHeight());
    graphHeight = std::min(static_cast<float>(width * 2 / M_PI), height);
    graphWidth = static_cast<float>(graphHeight * M_PI / 2);
    //origin: (oX, oY)
    oX = width / 2;
    oY = height / 2 - (height - graphHeight) / 2;
    float halfHeight = graphHeight / 2;
    float halfWidth = graphWidth / 2;
    g.setColour(colorFromHex(0xB0BEC5));
    g.drawLine(oX - halfWidth, oY, oX + halfWidth, oY, 0.7f);//X-Axis
    g.drawLine(oX, oY - halfHeight, oX, oY + halfHeight, 0.7f);//Y-Axis
    g.drawRect(oX - halfWidth, oY - halfHeight, graphWidth, graphHeight, 2.0f);
    if (!table->empty()) {
        drawGraph(g);
    }
}

void ShaperGraph::drawGraph(juce::Graphics &g) {
    g.setColour(colorFromHex(0xE53935));
    float step = (TO - FROM) / float(table->size() - 1);
    juce::Point<float> lastPoint;
    for (size_t i = 0; i < table->size(); ++i) {
        float x = FROM + float(i) * step;
        float y = (*table)[i];
        y = inRange(y, -1.0f, 1.0f);
        float realX = oX + x * zoomRate * graphWidth / (TO - FROM);
        float realY = oY - y * zoomRate * graphHeight / 2;
        if (i == 0) {
            g.drawLine(realX, realY, realX, realY, 2.5f);
        }
        else {
            g.drawLine(juce::Line(
                               lastPoint,
                               juce::Point{realX, realY}),
                       2.5f);
        }
        lastPoint = {realX, realY};
    }
}

void ShaperGraph::resized() {}

void ShaperGraph::setTable(std::vector<float> *_table) {
    table = _table;
}

}// namespace ZX