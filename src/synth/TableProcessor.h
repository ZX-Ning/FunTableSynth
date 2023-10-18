#ifndef ZX_FUN_TABLE_TABLE_PROC_H_
#define ZX_FUN_TABLE_TABLE_PROC_H_

#include <cmath>
#include <vector>

namespace ZX{

class TableProcessor{
private:
    static constexpr double FROM = -M_PI, TO = M_PI;
    std::vector<float> *table;
    float linearInterpolation(double index) const;
public:
    explicit TableProcessor(std::vector<float>*);
    float processSample(float) const;
};

}

#endif