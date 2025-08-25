#include "../CMODLoader.h"
#include "../include/Utils/zip_file.hpp"

namespace CHTL {

std::map<std::string, std::string> CMODLoader::Load(const std::string& cmodFile) {
    std::map<std::string, std::string> fileContents;
    miniz_cpp::zip_file file(cmodFile);

    for (const auto& member : file.infolist()) {
        // Check if the entry is a directory by checking if its filename ends with a slash.
        if (member.filename.empty() || (member.filename.back() != '/' && member.filename.back() != '\')) {
            fileContents[member.filename] = file.read(member);
        }
    }

    return fileContents;
}

}
