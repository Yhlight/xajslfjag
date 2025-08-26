#pragma once

#include <string>
#include <memory>
#include "CHTL_JS/Core/Ast.h"

namespace CHTL_JS {

    // printMylove converts an image to character pixel blocks or ASCII art
    class PrintMyloveHandler {
    public:
        static std::string Generate(const struct PrintMyloveNode& node);
        
    private:
        static std::string GeneratePixelMode(const std::string& url, int width, int height, double scale);
        static std::string GenerateASCIIMode(const std::string& url, int width, int height, double scale);
    };

}