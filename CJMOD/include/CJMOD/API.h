#pragma once

#include "CHTLJSFunction.h"

namespace CHTL {

    /**
     * @brief Makes a manually defined function compatible with the 'vir' object system.
     * @param functionName The name of the function to register.
     */
    void bindVir(const std::string& functionName);

}
