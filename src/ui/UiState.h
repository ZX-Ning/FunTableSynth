#ifndef ZX_FUNTABLE_SYNTH_UISTATE_H_
#define ZX_FUNTABLE_SYNTH_UISTATE_H_

#include <string>

namespace ZX {

struct UiState {
    std::string code;
    std::string errMsg;
    bool isButtonEnable;
    std::vector<float> currentTable;
};

}

#endif //ZX_FUNTABLE_SYNTH_UISTATE_H_
