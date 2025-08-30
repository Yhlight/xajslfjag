#pragma once

#include <string>
#include <vector>

namespace CHTL {

class FileSystem {
public:
    static bool exists(const std::string& path);
    static bool isFile(const std::string& path);
    static bool isDirectory(const std::string& path);
    
    static std::string readFile(const std::string& filePath);
    static bool writeFile(const std::string& filePath, const std::string& content);
    
    static bool createDirectory(const std::string& path);
    static std::vector<std::string> listFiles(const std::string& directory);
};

}