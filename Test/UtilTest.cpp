#include <iostream>
#include <cassert>
#include <filesystem>
#include <fstream>
#include "../CHTL/Utils/CMODPackager.h"
#include "../CHTL/Utils/CMODLoader.h"

namespace fs = std::filesystem;

void TestCMODPackaging() {
    std::cout << "Testing CMOD Packaging..." << std::endl;

    // 1. Setup a temporary directory structure for the test module
    fs::create_directories("temp_module/src");
    fs::create_directories("temp_module/info");
    std::ofstream( "temp_module/src/main.chtl") << "div{}";
    std::ofstream("temp_module/info/info.chtl") << "[Info]{}";

    // 2. Pack the directory
    bool packSuccess = CHTL::CMODPackager::Pack("temp_module", "test.cmod");
    assert(packSuccess && "Packing failed.");
    assert(fs::exists("test.cmod") && "Packed file does not exist.");

    // 3. Load the packed file
    auto loadedFiles = CHTL::CMODLoader::Load("test.cmod");
    assert(loadedFiles.size() == 2 && "Incorrect number of files loaded.");
    assert(loadedFiles["src/main.chtl"] == "div{}" && "File content mismatch.");

    // 4. Cleanup
    fs::remove_all("temp_module");
    fs::remove("test.cmod");

    std::cout << "CMOD Packaging Test Passed!" << std::endl;
}

int main() {
    try {
        TestCMODPackaging();
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}