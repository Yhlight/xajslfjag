#pragma once

#include <string>

namespace CHTL {

class ZIPUtil {
public:
    static bool compress(const std::string& sourceDir, const std::string& zipPath);
    static bool extract(const std::string& zipPath, const std::string& outputDir);
};

}