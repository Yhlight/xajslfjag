#include "JSFileStream.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include <sstream>
#include <iomanip>

namespace CHTLJS::IO {

JSFileStream::JSFileStream() 
    : fileStream_(nullptr)
    , optimizationEnabled_(false) {
    cache_.lastModified = 0;
}

JSFileStream::JSFileStream(const String& filePath) : JSFileStream() {
    currentFilePath_ = filePath;
}

JSFileStream::~JSFileStream() {
    if (fileStream_ && fileStream_->is_open()) {
        fileStream_->close();
    }
}

bool JSFileStream::openCHTLJSFile(const String& filePath) {
    currentFilePath_ = filePath;
    fileStream_ = std::make_unique<std::fstream>(filePath, std::ios::in | std::ios::binary);
    
    if (fileStream_ && fileStream_->is_open()) {
        updateFileCache();
        return true;
    }
    return false;
}

bool JSFileStream::openJSFile(const String& filePath) {
    return openCHTLJSFile(filePath); // 使用相同的打开逻辑
}

bool JSFileStream::openCJJSFile(const String& filePath) {
    return openCHTLJSFile(filePath); // 使用相同的打开逻辑
}

String JSFileStream::readCHTLJSContent() {
    if (!fileStream_ || !fileStream_->is_open()) {
        return "";
    }
    
    fileStream_->seekg(0, std::ios::end);
    size_t fileSize = fileStream_->tellg();
    fileStream_->seekg(0, std::ios::beg);
    
    String content;
    content.resize(fileSize);
    
    fileStream_->read(&content[0], fileSize);
    size_t actualRead = fileStream_->gcount();
    content.resize(actualRead);
    
    return content;
}

StringVector JSFileStream::extractCHTLJSBlocks() {
    String content = readCHTLJSContent();
    StringVector blocks;
    
    // 查找 {{...}} 块
    size_t pos = 0;
    while ((pos = content.find("{{", pos)) != String::npos) {
        size_t endPos = content.find("}}", pos + 2);
        if (endPos != String::npos) {
            String block = content.substr(pos, endPos - pos + 2);
            blocks.push_back(block);
            pos = endPos + 2;
        } else {
            break;
        }
    }
    
    return blocks;
}

StringVector JSFileStream::extractJSBlocks() {
    String content = readCHTLJSContent();
    return CHTLJSFileProcessor::splitIntoBlocks(content);
}

StringVector JSFileStream::extractModuleImports() {
    String content = readCHTLJSContent();
    StringVector imports;
    
    // 查找 module { } 块中的导入
    size_t pos = 0;
    while ((pos = content.find("module", pos)) != String::npos) {
        size_t openBrace = content.find("{", pos);
        if (openBrace != String::npos) {
            size_t closeBrace = content.find("}", openBrace);
            if (closeBrace != String::npos) {
                String moduleBlock = content.substr(openBrace + 1, closeBrace - openBrace - 1);
                
                // 提取 load: 语句
                size_t loadPos = 0;
                while ((loadPos = moduleBlock.find("load:", loadPos)) != String::npos) {
                    size_t lineEnd = moduleBlock.find_first_of(";\n", loadPos);
                    if (lineEnd != String::npos) {
                        String loadStatement = moduleBlock.substr(loadPos, lineEnd - loadPos);
                        imports.push_back(loadStatement);
                    }
                    loadPos = lineEnd != String::npos ? lineEnd + 1 : moduleBlock.length();
                }
                
                pos = closeBrace + 1;
            } else {
                break;
            }
        } else {
            pos += 6; // 跳过 "module"
        }
    }
    
    return imports;
}

String JSFileStream::extractModuleDefinition() {
    String content = readCHTLJSContent();
    
    size_t modulePos = content.find("module");
    if (modulePos != String::npos) {
        size_t openBrace = content.find("{", modulePos);
        if (openBrace != String::npos) {
            size_t closeBrace = content.find("}", openBrace);
            if (closeBrace != String::npos) {
                return content.substr(modulePos, closeBrace - modulePos + 1);
            }
        }
    }
    
    return "";
}

bool JSFileStream::writeModuleOutput(const String& content) {
    if (!fileStream_) {
        return false;
    }
    
    // 重新打开文件进行写入
    fileStream_->close();
    fileStream_ = std::make_unique<std::fstream>(currentFilePath_, std::ios::out | std::ios::trunc);
    
    if (fileStream_ && fileStream_->is_open()) {
        fileStream_->write(content.c_str(), content.length());
        fileStream_->flush();
        return fileStream_->good();
    }
    
    return false;
}

StringVector JSFileStream::extractCJMODSyntax() {
    String content = readCHTLJSContent();
    return CHTLJSFileProcessor::findCJMODKeywords(content);
}

bool JSFileStream::processCJMODFile(const String& inputPath, const String& outputPath) {
    JSFileStream inputStream(inputPath);
    if (!inputStream.openCJJSFile(inputPath)) {
        return false;
    }
    
    String content = inputStream.readCHTLJSContent();
    String processedContent = CHTLJSFileProcessor::processCJMODSyntax(content);
    
    JSFileStream outputStream(outputPath);
    fileStream_ = std::make_unique<std::fstream>(outputPath, std::ios::out | std::ios::trunc);
    
    if (fileStream_ && fileStream_->is_open()) {
        fileStream_->write(processedContent.c_str(), processedContent.length());
        fileStream_->flush();
        return fileStream_->good();
    }
    
    return false;
}

bool JSFileStream::hasFileChanged() const {
    if (currentFilePath_.empty()) {
        return false;
    }
    
    String currentHash = getFileHash();
    return currentHash != cache_.lastHash;
}

String JSFileStream::getFileHash() const {
    if (!fileStream_ || !fileStream_->is_open()) {
        return "";
    }
    
    String content = const_cast<JSFileStream*>(this)->readCHTLJSContent();
    return calculateMD5Hash(content);
}

void JSFileStream::updateFileCache() {
    cache_.lastHash = getFileHash();
    cache_.lastContent = readCHTLJSContent();
    cache_.lastModified = std::time(nullptr);
}

bool JSFileStream::validateCHTLJSSyntax(const String& content) {
    // 检查基本的语法结构
    if (!validateBraceMatching(content)) {
        return false;
    }
    
    // 检查CHTL JS特定语法
    StringVector blocks = extractCHTLJSBlocks();
    for (const auto& block : blocks) {
        if (block.find("{{") == String::npos || block.find("}}") == String::npos) {
            return false;
        }
    }
    
    return true;
}

StringVector JSFileStream::findSyntaxErrors(const String& content) {
    StringVector errors;
    
    if (!validateBraceMatching(content)) {
        errors.push_back("括号不匹配");
    }
    
    // 检查未闭合的CHTL JS块
    size_t openCount = 0, closeCount = 0;
    size_t pos = 0;
    
    while ((pos = content.find("{{", pos)) != String::npos) {
        openCount++;
        pos += 2;
    }
    
    pos = 0;
    while ((pos = content.find("}}", pos)) != String::npos) {
        closeCount++;
        pos += 2;
    }
    
    if (openCount != closeCount) {
        errors.push_back("CHTL JS块未正确闭合");
    }
    
    return errors;
}

void JSFileStream::enableCHTLJSOptimization(bool enable) {
    optimizationEnabled_ = enable;
}

bool JSFileStream::isCHTLJSOptimizationEnabled() const {
    return optimizationEnabled_;
}

String JSFileStream::calculateMD5Hash(const String& content) {
    // 简化的哈希计算（在实际实现中应该使用真正的MD5）
    std::hash<String> hasher;
    size_t hashValue = hasher(content);
    
    std::ostringstream oss;
    oss << std::hex << hashValue;
    return oss.str();
}

bool JSFileStream::validateBraceMatching(const String& content) {
    int braceCount = 0;
    int parenCount = 0;
    int bracketCount = 0;
    
    for (char c : content) {
        switch (c) {
            case '{': braceCount++; break;
            case '}': braceCount--; break;
            case '(': parenCount++; break;
            case ')': parenCount--; break;
            case '[': bracketCount++; break;
            case ']': bracketCount--; break;
        }
        
        if (braceCount < 0 || parenCount < 0 || bracketCount < 0) {
            return false;
        }
    }
    
    return braceCount == 0 && parenCount == 0 && bracketCount == 0;
}

// CHTLJSFileProcessor实现
CHTLJSFileProcessor::FileType CHTLJSFileProcessor::detectFileType(const String& filePath) {
    if (filePath.ends_with(".chtljs") || filePath.ends_with(".chtl")) {
        return FileType::CHTL_JS;
    } else if (filePath.ends_with(".cjjs")) {
        return FileType::CJJS;
    } else if (filePath.ends_with(".js")) {
        return FileType::PURE_JS;
    }
    return FileType::UNKNOWN;
}

CHTLJSFileProcessor::FileType CHTLJSFileProcessor::detectContentType(const String& content) {
    if (containsCHTLJSMarkers(content)) {
        return FileType::CHTL_JS;
    } else if (containsModuleSyntax(content)) {
        return FileType::MODULE;
    } else {
        return FileType::PURE_JS;
    }
}

String CHTLJSFileProcessor::preprocessCHTLJS(const String& content) {
    String processed = content;
    
    // 规范化行结束符
    processed = normalizeLineEndings(processed);
    
    // 其他预处理步骤...
    
    return processed;
}

String CHTLJSFileProcessor::extractCHTLJSScript(const String& content) {
    String result;
    size_t pos = 0;
    
    while ((pos = content.find("{{", pos)) != String::npos) {
        size_t endPos = content.find("}}", pos + 2);
        if (endPos != String::npos) {
            String block = content.substr(pos + 2, endPos - pos - 2);
            result += block + "\n";
            pos = endPos + 2;
        } else {
            break;
        }
    }
    
    return result;
}

StringVector CHTLJSFileProcessor::findCJMODKeywords(const String& content) {
    StringVector keywords;
    
    // 查找常见的CJMOD关键字
    StringVector searchKeywords = {"**", "await>>", "parallel>>", "timeout>>", "retry>>"};
    
    for (const auto& keyword : searchKeywords) {
        size_t pos = 0;
        while ((pos = content.find(keyword, pos)) != String::npos) {
            keywords.push_back(keyword);
            pos += keyword.length();
        }
    }
    
    return keywords;
}

bool CHTLJSFileProcessor::containsCHTLJSMarkers(const String& content) {
    return content.find("{{") != String::npos && content.find("}}") != String::npos;
}

bool CHTLJSFileProcessor::containsModuleSyntax(const String& content) {
    return content.find("module") != String::npos && content.find("load:") != String::npos;
}

String CHTLJSFileProcessor::normalizeLineEndings(const String& content) {
    String result;
    result.reserve(content.length());
    
    for (size_t i = 0; i < content.length(); ++i) {
        if (content[i] == '\r') {
            if (i + 1 < content.length() && content[i + 1] == '\n') {
                result += '\n';
                i++; // 跳过 \n
            } else {
                result += '\n';
            }
        } else {
            result += content[i];
        }
    }
    
    return result;
}

StringVector CHTLJSFileProcessor::splitIntoBlocks(const String& content) {
    StringVector blocks;
    
    std::istringstream iss(content);
    String line;
    String currentBlock;
    
    while (std::getline(iss, line)) {
        if (line.empty()) {
            if (!currentBlock.empty()) {
                blocks.push_back(currentBlock);
                currentBlock.clear();
            }
        } else {
            currentBlock += line + "\n";
        }
    }
    
    if (!currentBlock.empty()) {
        blocks.push_back(currentBlock);
    }
    
    return blocks;
}

} // namespace CHTLJS::IO