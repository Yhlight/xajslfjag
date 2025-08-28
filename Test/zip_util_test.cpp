#include <iostream>
#include <cassert>
#include <fstream>
#include <filesystem>
#include <vector>
#include <string>
#include "../Util/ZipUtil.h"

using namespace Util;

void testZipCreation() {
    std::cout << "=== 测试ZIP文件创建 ===" << std::endl;
    
    ZipUtil zipUtil;
    
    // 创建测试条目
    std::vector<ZipEntry> entries;
    
    // 文本文件条目
    ZipEntry textEntry;
    textEntry.fileName = "test.txt";
    textEntry.uncompressedSize = 15;
    textEntry.compressedSize = 15;
    textEntry.compression = 0; // 不压缩
    textEntry.data = {'H', 'e', 'l', 'l', 'o', ' ', 'W', 'o', 'r', 'l', 'd', '!', '\n', '\r', '\n'};
    textEntry.crc32 = ZipUtil::calculateCRC32(textEntry.data.data(), textEntry.data.size());
    entries.push_back(textEntry);
    
    // 二进制文件条目
    ZipEntry binEntry;
    binEntry.fileName = "data.bin";
    binEntry.uncompressedSize = 8;
    binEntry.compressedSize = 8;
    binEntry.compression = 0; // 不压缩
    binEntry.data = {0x00, 0x01, 0x02, 0x03, 0xFF, 0xFE, 0xFD, 0xFC};
    binEntry.crc32 = ZipUtil::calculateCRC32(binEntry.data.data(), binEntry.data.size());
    entries.push_back(binEntry);
    
    // 创建ZIP文件
    assert(zipUtil.createZip("test.zip", entries));
    std::cout << "ZIP文件创建成功！" << std::endl;
    
    // 验证ZIP文件存在
    assert(std::filesystem::exists("test.zip"));
    std::cout << "ZIP文件存在性验证通过！" << std::endl;
}

void testZipReading() {
    std::cout << "=== 测试ZIP文件读取 ===" << std::endl;
    
    ZipUtil zipUtil;
    
    // 读取ZIP文件
    assert(zipUtil.readZip("test.zip"));
    std::cout << "ZIP文件读取成功！" << std::endl;
    
    // 获取条目列表
    auto entries = zipUtil.getAllEntries();
    assert(entries.size() == 2);
    std::cout << "条目数量验证通过！" << std::endl;
    
    // 验证第一个条目
    assert(entries[0].fileName == "test.txt");
    assert(entries[0].uncompressedSize == 15);
    assert(entries[0].compressedSize == 15);
    assert(entries[0].compression == 0);
    assert(entries[0].data.size() == 15);
    std::cout << "第一个条目验证通过！" << std::endl;
    
    // 验证第二个条目
    assert(entries[1].fileName == "data.bin");
    assert(entries[1].uncompressedSize == 8);
    assert(entries[1].compressedSize == 8);
    assert(entries[1].compression == 0);
    assert(entries[1].data.size() == 8);
    std::cout << "第二个条目验证通过！" << std::endl;
}

void testZipExtraction() {
    std::cout << "=== 测试ZIP文件解压 ===" << std::endl;
    
    ZipUtil zipUtil;
    
    // 创建解压目录
    std::filesystem::create_directories("extract_test");
    
    // 解压ZIP文件
    assert(zipUtil.extractZip("test.zip", "extract_test"));
    std::cout << "ZIP文件解压成功！" << std::endl;
    
    // 验证解压的文件
    assert(std::filesystem::exists("extract_test/test.txt"));
    assert(std::filesystem::exists("extract_test/data.bin"));
    std::cout << "解压文件存在性验证通过！" << std::endl;
    
    // 验证文件内容
    std::ifstream textFile("extract_test/test.txt", std::ios::binary);
    assert(textFile.is_open());
    std::string textContent((std::istreambuf_iterator<char>(textFile)), std::istreambuf_iterator<char>());
    textFile.close();
    
    assert(textContent == "Hello World!\n\r\n");
    std::cout << "文本文件内容验证通过！" << std::endl;
    
    std::ifstream binFile("extract_test/data.bin", std::ios::binary);
    assert(binFile.is_open());
    std::vector<uint8_t> binContent((std::istreambuf_iterator<char>(binFile)), std::istreambuf_iterator<char>());
    binFile.close();
    
    assert(binContent.size() == 8);
    assert(binContent[0] == 0x00);
    assert(binContent[7] == 0xFC);
    std::cout << "二进制文件内容验证通过！" << std::endl;
}

void testZipInfo() {
    std::cout << "=== 测试ZIP文件信息 ===" << std::endl;
    
    ZipUtil zipUtil;
    
    // 获取ZIP文件信息
    std::string info = zipUtil.getZipInfo("test.zip");
    assert(!info.empty());
    assert(info.find("ZIP文件信息") != std::string::npos);
    assert(info.find("总文件数: 2") != std::string::npos);
    assert(info.find("test.txt") != std::string::npos);
    assert(info.find("data.bin") != std::string::npos);
    
    std::cout << "ZIP文件信息:\n" << info << std::endl;
    std::cout << "ZIP文件信息验证通过！" << std::endl;
}

void testZipVerification() {
    std::cout << "=== 测试ZIP文件验证 ===" << std::endl;
    
    ZipUtil zipUtil;
    
    // 验证ZIP文件完整性
    assert(zipUtil.verifyZip("test.zip"));
    std::cout << "ZIP文件完整性验证通过！" << std::endl;
}

void testZipModification() {
    std::cout << "=== 测试ZIP文件修改 ===" << std::endl;
    
    ZipUtil zipUtil;
    
    // 创建测试文件
    std::ofstream testFile("new_file.txt");
    testFile << "This is a new file for testing ZIP modification.";
    testFile.close();
    
    // 添加文件到ZIP
    assert(zipUtil.addFileToZip("new_file.txt", "test.zip"));
    std::cout << "文件添加成功！" << std::endl;
    
    // 验证文件已添加
    auto entries = zipUtil.getAllEntries();
    std::cout << "添加文件后的条目数量: " << entries.size() << std::endl;
    for (const auto& entry : entries) {
        std::cout << "  - " << entry.fileName << std::endl;
    }
    assert(entries.size() == 3);
    
    bool foundNewFile = false;
    for (const auto& entry : entries) {
        if (entry.fileName == "new_file.txt") {
            foundNewFile = true;
            break;
        }
    }
    assert(foundNewFile);
    std::cout << "新文件验证通过！" << std::endl;
    
    // 删除文件
    assert(zipUtil.removeFileFromZip("new_file.txt", "test.zip"));
    std::cout << "文件删除成功！" << std::endl;
    
    // 验证文件已删除
    entries = zipUtil.getAllEntries();
    assert(entries.size() == 2);
    
    foundNewFile = false;
    for (const auto& entry : entries) {
        if (entry.fileName == "new_file.txt") {
            foundNewFile = true;
            break;
        }
    }
    assert(!foundNewFile);
    std::cout << "文件删除验证通过！" << std::endl;
}

void testCompression() {
    std::cout << "=== 测试数据压缩 ===" << std::endl;
    
    ZipUtil zipUtil;
    
    // 创建可压缩的数据（重复字节）
    std::vector<uint8_t> originalData;
    for (int i = 0; i < 100; ++i) {
        originalData.push_back(0xAA);
    }
    
    // 压缩数据
    std::vector<uint8_t> compressedData;
    assert(zipUtil.compressData(originalData, compressedData));
    std::cout << "数据压缩成功！" << std::endl;
    
    // 验证压缩效果
    assert(compressedData.size() < originalData.size());
    std::cout << "压缩效果验证通过！原始大小: " << originalData.size() 
              << ", 压缩后大小: " << compressedData.size() << std::endl;
    
    // 解压数据
    std::vector<uint8_t> decompressedData;
    assert(zipUtil.decompressData(compressedData, decompressedData));
    std::cout << "数据解压成功！" << std::endl;
    
    // 验证解压结果
    assert(decompressedData.size() == originalData.size());
    for (size_t i = 0; i < originalData.size(); ++i) {
        assert(decompressedData[i] == originalData[i]);
    }
    std::cout << "解压结果验证通过！" << std::endl;
}

void testStatistics() {
    std::cout << "=== 测试统计信息 ===" << std::endl;
    
    ZipUtil zipUtil;
    
    // 获取统计信息
    std::string stats = zipUtil.getStatistics();
    assert(!stats.empty());
    assert(stats.find("ZipUtil统计信息") != std::string::npos);
    
    std::cout << "统计信息:\n" << stats << std::endl;
    
    // 清除统计信息
    zipUtil.clearStatistics();
    stats = zipUtil.getStatistics();
    assert(stats.find("总ZIP文件创建数: 0") != std::string::npos);
    
    std::cout << "统计信息清除验证通过！" << std::endl;
}

void testErrorHandling() {
    std::cout << "=== 测试错误处理 ===" << std::endl;
    
    ZipUtil zipUtil;
    
    // 测试读取不存在的ZIP文件
    assert(!zipUtil.readZip("nonexistent.zip"));
    std::cout << "不存在的ZIP文件处理验证通过！" << std::endl;
    
    // 测试解压不存在的ZIP文件
    assert(!zipUtil.extractZip("nonexistent.zip", "extract_test"));
    std::cout << "不存在的ZIP文件解压处理验证通过！" << std::endl;
    
    // 测试获取不存在的ZIP文件信息
    std::string info = zipUtil.getZipInfo("nonexistent.zip");
    assert(info == "Failed to read ZIP file");
    std::cout << "不存在的ZIP文件信息获取处理验证通过！" << std::endl;
}

void cleanup() {
    std::cout << "=== 清理测试文件 ===" << std::endl;
    
    // 删除测试文件
    if (std::filesystem::exists("test.zip")) {
        std::filesystem::remove("test.zip");
    }
    
    if (std::filesystem::exists("new_file.txt")) {
        std::filesystem::remove("new_file.txt");
    }
    
    if (std::filesystem::exists("extract_test")) {
        std::filesystem::remove_all("extract_test");
    }
    
    std::cout << "测试文件清理完成！" << std::endl;
}

int main() {
    std::cout << "开始ZipUtil测试...\n" << std::endl;
    
    try {
        testZipCreation();
        testZipReading();
        testZipExtraction();
        testZipInfo();
        testZipVerification();
        testZipModification();
        testCompression();
        testStatistics();
        testErrorHandling();
        
        std::cout << "\n所有测试通过！" << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "测试失败: " << e.what() << std::endl;
        return 1;
    }
    
    cleanup();
    
    std::cout << "\nZipUtil测试完成！" << std::endl;
    return 0;
}