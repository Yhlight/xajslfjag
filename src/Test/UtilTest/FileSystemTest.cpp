#include "../CHTLTestSuite.h"
#include "../../CHTL/CHTLIOStream/CHTLFileSystem.h"
#include <fstream>
#include <filesystem>

using namespace CHTL::Test;

CHTL_TEST_SUITE(FileSystem) {
    CHTL_ADD_TEST(FileSystem, PathOperations);
    CHTL_ADD_TEST(FileSystem, FileOperations);
    CHTL_ADD_TEST(FileSystem, DirectoryOperations);
    CHTL_ADD_TEST(FileSystem, PathNormalization);
    CHTL_ADD_TEST(FileSystem, GlobMatching);
}

CHTL_TEST(FileSystem, PathOperations) {
    // 测试路径组合
    assertEqual(PathUtil::join("dir", "file.txt"), "dir/file.txt");
    assertEqual(PathUtil::join("dir/", "file.txt"), "dir/file.txt");
    assertEqual(PathUtil::join("", "file.txt"), "file.txt");
    
    // 测试扩展名操作
    assertEqual(PathUtil::extension("file.txt"), ".txt");
    assertEqual(PathUtil::extension("file"), "");
    assertEqual(PathUtil::extension("file.tar.gz"), ".gz");
    
    // 测试文件名
    assertEqual(PathUtil::filename("/path/to/file.txt"), "file.txt");
    assertEqual(PathUtil::filename("file.txt"), "file.txt");
    
    // 测试目录名
    assertEqual(PathUtil::dirname("/path/to/file.txt"), "/path/to");
    assertEqual(PathUtil::dirname("file.txt"), ".");
    
    // 测试替换扩展名
    assertEqual(PathUtil::replaceExtension("file.txt", ".html"), "file.html");
    assertEqual(PathUtil::replaceExtension("file", ".html"), "file.html");
}

CHTL_TEST(FileSystem, FileOperations) {
    const std::string testFile = "test_file.txt";
    const std::string testContent = "Hello, World!";
    
    // 清理
    if (FileSystem::exists(testFile)) {
        FileSystem::remove(testFile);
    }
    
    // 测试文件不存在
    assertFalse(FileSystem::exists(testFile));
    
    // 创建并写入文件
    {
        std::ofstream ofs(testFile);
        ofs << testContent;
    }
    
    // 测试文件存在
    assertTrue(FileSystem::exists(testFile));
    assertTrue(FileSystem::isFile(testFile));
    assertFalse(FileSystem::isDirectory(testFile));
    
    // 测试文件大小
    assertEqual(FileSystem::fileSize(testFile), testContent.size());
    
    // 测试读取文件
    std::string content = FileSystem::readFile(testFile);
    assertEqual(content, testContent);
    
    // 测试复制文件
    const std::string copyFile = "test_file_copy.txt";
    assertTrue(FileSystem::copy(testFile, copyFile));
    assertTrue(FileSystem::exists(copyFile));
    assertEqual(FileSystem::readFile(copyFile), testContent);
    
    // 清理
    FileSystem::remove(testFile);
    FileSystem::remove(copyFile);
    assertFalse(FileSystem::exists(testFile));
    assertFalse(FileSystem::exists(copyFile));
}

CHTL_TEST(FileSystem, DirectoryOperations) {
    const std::string testDir = "test_directory";
    const std::string nestedDir = "test_directory/nested/deep";
    
    // 清理
    if (FileSystem::exists(testDir)) {
        FileSystem::removeRecursive(testDir);
    }
    
    // 测试目录不存在
    assertFalse(FileSystem::exists(testDir));
    
    // 创建目录
    assertTrue(FileSystem::createDirectory(testDir));
    assertTrue(FileSystem::exists(testDir));
    assertTrue(FileSystem::isDirectory(testDir));
    assertFalse(FileSystem::isFile(testDir));
    
    // 创建嵌套目录
    assertTrue(FileSystem::createDirectories(nestedDir));
    assertTrue(FileSystem::exists(nestedDir));
    
    // 测试列出目录内容
    std::ofstream(testDir + "/file1.txt") << "test1";
    std::ofstream(testDir + "/file2.txt") << "test2";
    FileSystem::createDirectory(testDir + "/subdir");
    
    auto entries = FileSystem::listDirectory(testDir);
    assertEqual(entries.size(), 4);  // nested, file1.txt, file2.txt, subdir
    
    // 清理
    FileSystem::removeRecursive(testDir);
    assertFalse(FileSystem::exists(testDir));
}

CHTL_TEST(FileSystem, PathNormalization) {
    // 测试路径规范化
    assertEqual(PathUtil::normalize("./file.txt"), "file.txt");
    assertEqual(PathUtil::normalize("dir/../file.txt"), "file.txt");
    assertEqual(PathUtil::normalize("dir/./file.txt"), "dir/file.txt");
    assertEqual(PathUtil::normalize("/dir/../file.txt"), "/file.txt");
    
    // 测试相对路径
    assertEqual(PathUtil::makeRelative("/home/user/project", "/home/user/project/src/main.cpp"), 
                "src/main.cpp");
    assertEqual(PathUtil::makeRelative("/home/user", "/home/user/file.txt"), "file.txt");
    
    // 测试绝对路径
    assertTrue(PathUtil::isAbsolute("/home/user"));
    assertFalse(PathUtil::isAbsolute("home/user"));
    #ifdef _WIN32
    assertTrue(PathUtil::isAbsolute("C:\\Users"));
    #endif
}

CHTL_TEST(FileSystem, GlobMatching) {
    // 创建测试文件结构
    const std::string testDir = "glob_test";
    FileSystem::createDirectories(testDir + "/src");
    FileSystem::createDirectories(testDir + "/include");
    
    std::ofstream(testDir + "/src/main.cpp") << "";
    std::ofstream(testDir + "/src/util.cpp") << "";
    std::ofstream(testDir + "/src/test.cc") << "";
    std::ofstream(testDir + "/include/header.h") << "";
    std::ofstream(testDir + "/README.md") << "";
    
    // 测试glob匹配
    auto cppFiles = FileSystem::glob(testDir + "/src/*.cpp");
    assertEqual(cppFiles.size(), 2);
    
    auto allSrcFiles = FileSystem::glob(testDir + "/src/*");
    assertEqual(allSrcFiles.size(), 3);
    
    auto allFiles = FileSystem::glob(testDir + "/**/*");
    assertTrue(allFiles.size() >= 5);
    
    // 清理
    FileSystem::removeRecursive(testDir);
}