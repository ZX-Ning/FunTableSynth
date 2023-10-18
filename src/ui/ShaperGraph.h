#ifndef ZX_FUN_TABLE_SYNTH_UI_SHAPER_GRAPH_H
#define ZX_FUN_TABLE_SYNTH_UI_SHAPER_GRAPH_H

#include <JuceHeader.h>
#include <vector>
namespace ZX {

class ShaperGraph : public juce::Component{
private:
    std::vector<float> *table;
    float width, height;
    float oX, oY, graphHeight, graphWidth;
    float zoomRate{0.985f};
public:
    ShaperGraph(std::vector<float>*);
    void paint(juce::Graphics &g) override;
    void drawGraph(juce::Graphics &g);
    void resized() override;
    void setTable(std::vector<float> *);
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ShaperGraph)
};

}

#endif //ZX_FUN_TABLE_SYNTH_UI_SHAPER_GRAPH_H
