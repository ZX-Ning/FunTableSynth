#include "TableProcessor.h"
#include "../helpers.h"

namespace ZX{

TableProcessor::TableProcessor(std::vector<float>* _tableP): table(_tableP){
}

float TableProcessor::processSample(float x) const {
    if (table->empty()) {
        return 0;
    }
    size_t tableSize = table->size();
    //size == floor((to - fr) / step) + 1
    double step = (TO - FROM) / double(tableSize - 1);
    double index = (x - FROM) / step;
    float result = linearInterpolation(index);
    return inRange(result, -1.0f, 1.0f);
}

float TableProcessor::linearInterpolation(double index) const {
    // point in the left: (x1, y1); point in the right; (x2,y2); the result point (index, result)
    size_t x1 = static_cast<size_t>(std::floor(index));
    size_t x2 = static_cast<size_t>(std::ceil(index));
    // prevent out of range
    if (x2 >= table->size() || x1 >= table->size()) {
        return table->back();
    }
    double y1 = (*table)[x1];
    double y2 = (*table)[x2];
    if (x2 == x1) {
        return static_cast<float>((y1 + y2) / 2);
    }
    double slope = (y2 - y1) / double(x2 - x1);
    return static_cast<float>(slope * (index - double(x1)) + y1);
}

}