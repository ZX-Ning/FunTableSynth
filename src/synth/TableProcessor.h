#ifndef ZX_FUN_TABLE_TABLE_PROC_H_
#define ZX_FUN_TABLE_TABLE_PROC_H_
#include "../helpers.h"
#include <vector>

namespace ZX{

class TableProcessor{
private:
    static constexpr double FROM = -PI, TO = PI;
    std::vector<float> *table;
    float linearInterpolation(double index) const;
public:
    explicit TableProcessor(std::vector<float>*);
    float processSample(float) const;
};

}

#endif