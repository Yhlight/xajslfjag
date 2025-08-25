#include "../CJMODPackager.h"
#include "../CHTL/Utils/include/Utils/zip_file.hpp"
#include <filesystem>

namespace CHTL {

namespace fs = std::filesystem;

bool CJMODPackager::Pack(const std::string& sourceDir, const std::string& destinationFile) {
    fs::path srcPath(sourceDir);
    if (!fs::exists(srcPath) || !fs::is_directory(srcPath)) {
        return false;
    }

    miniz_cpp::zip_file file;

    for (const auto& entry : fs::recursive_directory_iterator(srcPath)) {
        if (entry.is_regular_file()) {
            file.write(entry.path().string(), fs::relative(entry.path(), srcPath).string());
        }
    }

    file.save(destinationFile);
    return true;
}

}
