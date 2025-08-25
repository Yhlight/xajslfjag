#include "../CMODPackager.h"
#include "../include/Utils/zip_file.hpp"
#include <filesystem>

namespace CHTL {

namespace fs = std::filesystem;

bool CMODPackager::Pack(const std::string& sourceDir, const std::string& destinationFile) {
    fs::path srcPath(sourceDir);
    if (!fs::exists(srcPath) || !fs::is_directory(srcPath)) {
        return false; // Source directory does not exist
    }

    fs::path srcFolderPath = srcPath / "src";
    fs::path infoFolderPath = srcPath / "info";

    if (!fs::exists(srcFolderPath) || !fs::exists(infoFolderPath)) {
        return false; // Required folders not found
    }

    miniz_cpp::zip_file file;

    // Add files from src directory
    for (const auto& entry : fs::recursive_directory_iterator(srcFolderPath)) {
        if (entry.is_regular_file()) {
            file.write(entry.path().string(), fs::relative(entry.path(), srcPath).string());
        }
    }

    // Add files from info directory
    for (const auto& entry : fs::recursive_directory_iterator(infoFolderPath)) {
        if (entry.is_regular_file()) {
            file.write(entry.path().string(), fs::relative(entry.path(), srcPath).string());
        }
    }

    file.save(destinationFile);
    return true;
}

}
