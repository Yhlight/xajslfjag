#include <iostream>
#include <string>
#include <vector>
#include <memory>

// 包含I/O流模块
#include "../Util/CHTLIOStream/CHTLIOStream.h"
#include "../Util/CHTLJSIOStream/CHTLJSIOStream.h"
#include "../Util/FileStream/FileStream.h"
#include "../Util/ZipUtil.h"

using namespace std;

void testCHTLIOStream() {
    cout << "=== 测试CHTLIOStream ===" << endl;
    
    // 创建CHTL I/O流
    CHTL::CHTLIOStream chtlStream(CHTL::CHTLIOStream::StreamMode::WRITE);
    
    // 打开文件
    if (chtlStream.open("test_chtl.txt")) {
        cout << "✓ CHTL文件打开成功" << endl;
        
        // 写入CHTL内容
        string chtlContent = R"([Template]
@Element div {
    @Var title = "Hello World";
    <h1>{{title}}</h1>
    <p>这是一个CHTL模板</p>
}

[Custom]
@Style {
    .custom-class {
        color: blue;
        font-size: 16px;
    }
})";
        
        if (chtlStream.writeCHTL(chtlContent)) {
            cout << "✓ CHTL内容写入成功" << endl;
        } else {
            cout << "✗ CHTL内容写入失败: " << chtlStream.getLastError() << endl;
        }
        
        chtlStream.close();
    } else {
        cout << "✗ CHTL文件打开失败: " << chtlStream.getLastError() << endl;
    }
    
    // 测试读取
    CHTL::CHTLIOStream chtlReadStream(CHTL::CHTLIOStream::StreamMode::READ);
    if (chtlReadStream.open("test_chtl.txt")) {
        cout << "✓ CHTL文件读取打开成功" << endl;
        
        string readContent = chtlReadStream.readCHTL();
        cout << "✓ 读取到的内容长度: " << readContent.length() << " 字符" << endl;
        
        chtlReadStream.close();
    }
    
    cout << endl;
}

void testCHTLJSIOStream() {
    cout << "=== 测试CHTLJSIOStream ===" << endl;
    
    // 创建CHTL JS I/O流
    CHTLJS::CHTLJSIOStream chtljsStream(CHTLJS::CHTLJSIOStream::StreamMode::WRITE);
    
    // 打开文件
    if (chtljsStream.open("test_chtljs.txt")) {
        cout << "✓ CHTL JS文件打开成功" << endl;
        
        // 写入CHTL JS内容
        string chtljsContent = R"(vir test = listen {
    click: () => {
        console.log("Box clicked!");
    },
    hover: () => {
        console.log("Hovered!");
    }
};

@Style {
    .box {
        width: 100px;
        height: 100px;
        background: red;
    }
})";
        
        if (chtljsStream.writeCHTLJS(chtljsContent)) {
            cout << "✓ CHTL JS内容写入成功" << endl;
        } else {
            cout << "✗ CHTL JS内容写入失败: " << chtljsStream.getLastError() << endl;
        }
        
        chtljsStream.close();
    } else {
        cout << "✗ CHTL JS文件打开失败: " << chtljsStream.getLastError() << endl;
    }
    
    // 测试读取
    CHTLJS::CHTLJSIOStream chtljsReadStream(CHTLJS::CHTLJSIOStream::StreamMode::READ);
    if (chtljsReadStream.open("test_chtljs.txt")) {
        cout << "✓ CHTL JS文件读取打开成功" << endl;
        
        string readContent = chtljsReadStream.readCHTLJS();
        cout << "✓ 读取到的内容长度: " << readContent.length() << " 字符" << endl;
        
        chtljsReadStream.close();
    }
    
    cout << endl;
}

void testFileStream() {
    cout << "=== 测试FileStream ===" << endl;
    
    // 测试文本文件流
    Util::TextFileStream textStream("test_text.txt", Util::FileStream::OpenMode::WRITE_ONLY);
    
    if (textStream.open()) {
        cout << "✓ 文本文件流打开成功" << endl;
        
        // 写入内容
        string textContent = "第一行内容\n第二行内容\n第三行内容";
        if (textStream.writeAll(textContent)) {
            cout << "✓ 文本内容写入成功" << endl;
        }
        
        textStream.close();
    }
    
    // 测试读取
    Util::TextFileStream textReadStream("test_text.txt", Util::FileStream::OpenMode::READ_ONLY);
    if (textReadStream.open()) {
        cout << "✓ 文本文件读取打开成功" << endl;
        
        string readContent = textReadStream.readAll();
        cout << "✓ 读取到的内容: " << readContent << endl;
        
        // 测试行操作
        cout << "✓ 文件总行数: " << textReadStream.getLineCount() << endl;
        
        // 测试插入行
        if (textReadStream.insertLine(1, "插入的新行")) {
            cout << "✓ 插入行成功" << endl;
        }
        
        textReadStream.close();
    }
    
    // 测试二进制文件流
    Util::BinaryFileStream binaryStream("test_binary.bin", Util::FileStream::OpenMode::WRITE_ONLY);
    if (binaryStream.open()) {
        cout << "✓ 二进制文件流打开成功" << endl;
        
        // 写入二进制数据
        uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05};
        size_t written = binaryStream.write(data, sizeof(data));
        cout << "✓ 写入二进制数据: " << written << " 字节" << endl;
        
        binaryStream.close();
    }
    
    cout << endl;
}

void testZipUtil() {
    cout << "=== 测试ZipUtil ===" << endl;
    
    Util::ZipUtil zipUtil;
    
    // 创建测试文件
    ofstream testFile1("test_chtl.txt");
    testFile1 << "这是一个CHTL测试文件";
    testFile1.close();
    
    ofstream testFile2("test_chtljs.txt");
    testFile2 << "这是一个CHTL JS测试文件";
    testFile2.close();
    
    // 创建ZIP条目
    vector<Util::ZipEntry> entries;
    
    // 读取文件内容
    ifstream file1("test_chtl.txt", ios::binary);
    if (file1.is_open()) {
        file1.seekg(0, ios::end);
        size_t size1 = file1.tellg();
        file1.seekg(0, ios::beg);
        
        vector<uint8_t> data1(size1);
        file1.read(reinterpret_cast<char*>(data1.data()), size1);
        file1.close();
        
        Util::ZipEntry entry1;
        entry1.fileName = "chtl_file.txt";
        entry1.uncompressedSize = size1;
        entry1.compressedSize = size1;
        entry1.compression = 0;
        entry1.crc32 = 0x12345678;
        entry1.data = data1;
        entries.push_back(entry1);
    }
    
    ifstream file2("test_chtljs.txt", ios::binary);
    if (file2.is_open()) {
        file2.seekg(0, ios::end);
        size_t size2 = file2.tellg();
        file2.seekg(0, ios::beg);
        
        vector<uint8_t> data2(size2);
        file2.read(reinterpret_cast<char*>(data2.data()), size2);
        file2.close();
        
        Util::ZipEntry entry2;
        entry2.fileName = "chtljs_file.txt";
        entry2.uncompressedSize = size2;
        entry2.compressedSize = size2;
        entry2.compression = 0;
        entry2.crc32 = 0x87654321;
        entry2.data = data2;
        entries.push_back(entry2);
    }
    
    // 创建ZIP文件
    if (zipUtil.createZip("test_archive.zip", entries)) {
        cout << "✓ ZIP文件创建成功" << endl;
    } else {
        cout << "✗ ZIP文件创建失败" << endl;
    }
    
    // 列出ZIP内容
    vector<Util::ZipEntry> zipEntries = zipUtil.listZipContents("test_archive.zip");
    cout << "✓ ZIP文件包含 " << zipEntries.size() << " 个条目:" << endl;
    
    for (const auto& entry : zipEntries) {
        cout << "  - " << entry.fileName 
             << " (压缩: " << entry.compressedSize 
             << " 字节, 原始: " << entry.uncompressedSize << " 字节)" << endl;
    }
    
    // 获取ZIP信息
    string zipInfo = zipUtil.getZipInfo("test_archive.zip");
    cout << "✓ ZIP文件信息:" << endl;
    cout << zipInfo << endl;
    
    // 验证ZIP完整性
    if (zipUtil.verifyZip("test_archive.zip")) {
        cout << "✓ ZIP文件完整性验证通过" << endl;
    } else {
        cout << "✗ ZIP文件完整性验证失败" << endl;
    }
    
    // 清理测试文件
    remove("test_chtl.txt");
    remove("test_chtljs.txt");
    remove("test_archive.zip");
    
    cout << endl;
}

void testTemplateAndConfigStreams() {
    cout << "=== 测试模板和配置流 ===" << endl;
    
    // 测试CHTL模板流
    CHTL::CHTLTemplateIOStream templateStream(CHTL::CHTLIOStream::StreamMode::WRITE);
    if (templateStream.open("test_template.chtl")) {
        cout << "✓ 模板文件打开成功" << endl;
        
        templateStream.setTemplateName("TestTemplate");
        string templateContent = "@Element div {\n    <h1>模板标题</h1>\n    <p>模板内容</p>\n}";
        
        if (templateStream.writeTemplate(templateContent)) {
            cout << "✓ 模板内容写入成功" << endl;
        }
        
        templateStream.close();
    }
    
    // 测试CHTL配置流
    CHTL::CHTLConfigIOStream configStream(CHTL::CHTLIOStream::StreamMode::WRITE);
    if (configStream.open("test_config.ini")) {
        cout << "✓ 配置文件打开成功" << endl;
        
        if (configStream.writeConfig("app_name", "CHTL应用")) {
            cout << "✓ 配置项写入成功" << endl;
        }
        
        if (configStream.writeConfig("version", "1.0.0")) {
            cout << "✓ 版本配置写入成功" << endl;
        }
        
        if (configStream.writeConfig("debug", "true")) {
            cout << "✓ 调试配置写入成功" << endl;
        }
        
        configStream.close();
    }
    
    // 测试CHTL JS模块流
    CHTLJS::CHTLJSModuleIOStream moduleStream(CHTLJS::CHTLJSIOStream::StreamMode::WRITE);
    if (moduleStream.open("test_module.cjs")) {
        cout << "✓ 模块文件打开成功" << endl;
        
        moduleStream.setModuleName("TestModule");
        moduleStream.addDependency("CoreModule");
        moduleStream.addDependency("UtilsModule");
        
        string moduleContent = "// 测试模块\nvir test = {\n    name: 'test',\n    version: '1.0.0'\n};";
        
        if (moduleStream.writeModule(moduleContent)) {
            cout << "✓ 模块内容写入成功" << endl;
        }
        
        moduleStream.close();
    }
    
    cout << endl;
}

int main() {
    cout << "CHTL项目 I/O流系统测试程序" << endl;
    cout << "================================" << endl << endl;
    
    try {
        testCHTLIOStream();
        testCHTLJSIOStream();
        testFileStream();
        testZipUtil();
        testTemplateAndConfigStreams();
        
        cout << "所有测试完成！" << endl;
        
    } catch (const exception& e) {
        cerr << "测试过程中发生异常: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}