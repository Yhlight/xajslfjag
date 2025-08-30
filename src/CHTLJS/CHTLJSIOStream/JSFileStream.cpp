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

JSFileStream::JSFileStream(const CHTL::String& filePath) : JSFileStream() {
    currentFilePath_ = filePath;
}

JSFileStream::~JSFileStream() {
    if (fileStream_ && fileStream_->is_open()) {
        fileStream_->close();
    }
}

bool JSFileStream::openCHTLJSFile(const CHTL::String& filePath) {
    currentFilePath_ = filePath;
    fileStream_ = std::make_unique<std::fstream>(filePath, std::ios::in | std::ios::binary);
    
    if (fileStream_ && fileStream_->is_open()) {
        updateFileCache();
        return true;
    }
    return false;
}

bool JSFileStream::openJSFile(const CHTL::String& filePath) {
    return openCHTLJSFile(filePath); // 使用相同的打开逻辑
}

bool JSFileStream::openCJJSFile(const CHTL::String& filePath) {
    return openCHTLJSFile(filePath); // 使用相同的打开逻辑
}

CHTL::String JSFileStream::readCHTLJSContent() {
    if (!fileStream_ || !fileStream_->is_open()) {
        return "";
    }
    
    fileStream_->seekg(0, std::ios::end);
    size_t fileSize = fileStream_->tellg();
    fileStream_->seekg(0, std::ios::beg);
    
    CHTL::String content;
    content.resize(fileSize);
    
    fileStream_->read(&content[0], fileSize);
    size_t actualRead = fileStream_->gcount();
    content.resize(actualRead);
    
    return content;
}

CHTL::StringVector JSFileStream::extractCHTLJSBlocks() {
    CHTL::String content = readCHTLJSContent();
    CHTL::StringVector blocks;
    
    // 查找 {{...}} 块
    size_t pos = 0;
    while ((pos = content.find("{{", pos)) != CHTL::String::npos) {
        size_t endPos = content.find("}}", pos + 2);
        if (endPos != CHTL::String::npos) {
            CHTL::String block = content.substr(pos, endPos - pos + 2);
            blocks.push_back(block);
            pos = endPos + 2;
        } else {
            break;
        }
    }
    
    return blocks;
}

CHTL::StringVector JSFileStream::extractJSBlocks() {
    CHTL::String content = readCHTLJSContent();
    return CHTLJSFileProcessor::splitIntoBlocks(content);
}

CHTL::StringVector JSFileStream::extractModuleImports() {
    CHTL::String content = readCHTLJSContent();
    CHTL::StringVector imports;
    
    // 查找 module { } 块中的导入
    size_t pos = 0;
    while ((pos = content.find("module", pos)) != CHTL::String::npos) {
        size_t openBrace = content.find("{", pos);
        if (openBrace != CHTL::String::npos) {
            size_t closeBrace = content.find("}", openBrace);
            if (closeBrace != CHTL::String::npos) {
                CHTL::String moduleBlock = content.substr(openBrace + 1, closeBrace - openBrace - 1);
                
                // 提取 load: 语句
                size_t loadPos = 0;
                while ((loadPos = moduleBlock.find("load:", loadPos)) != CHTL::String::npos) {
                    size_t lineEnd = moduleBlock.find_first_of(";\n", loadPos);
                    if (lineEnd != CHTL::String::npos) {
                        CHTL::String loadStatement = moduleBlock.substr(loadPos, lineEnd - loadPos);
                        imports.push_back(loadStatement);
                    }
                    loadPos = lineEnd != CHTL::String::npos ? lineEnd + 1 : moduleBlock.length();
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

CHTL::String JSFileStream::extractModuleDefinition() {
    CHTL::String content = readCHTLJSContent();
    
    size_t modulePos = content.find("module");
    if (modulePos != CHTL::String::npos) {
        size_t openBrace = content.find("{", modulePos);
        if (openBrace != CHTL::String::npos) {
            size_t closeBrace = content.find("}", openBrace);
            if (closeBrace != CHTL::String::npos) {
                return content.substr(modulePos, closeBrace - modulePos + 1);
            }
        }
    }
    
    return "";
}

bool JSFileStream::writeModuleOutput(const CHTL::String& content) {
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

CHTL::StringVector JSFileStream::extractCJMODSyntax() {
    CHTL::String content = readCHTLJSContent();
    return CHTLJSFileProcessor::findCJMODKeywords(content);
}

bool JSFileStream::processCJMODFile(const CHTL::String& inputPath, const CHTL::String& outputPath) {
    JSFileStream inputStream(inputPath);
    if (!inputStream.openCJJSFile(inputPath)) {
        return false;
    }
    
    CHTL::String content = inputStream.readCHTLJSContent();
    CHTL::String processedContent = CHTLJSFileProcessor::processCJMODSyntax(content);
    
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
    
    CHTL::String currentHash = getFileHash();
    return currentHash != cache_.lastHash;
}

CHTL::String JSFileStream::getFileHash() const {
    if (!fileStream_ || !fileStream_->is_open()) {
        return "";
    }
    
    CHTL::String content = const_cast<JSFileStream*>(this)->readCHTLJSContent();
    return calculateMD5Hash(content);
}

void JSFileStream::updateFileCache() {
    cache_.lastHash = getFileHash();
    cache_.lastContent = readCHTLJSContent();
    cache_.lastModified = std::time(nullptr);
}

bool JSFileStream::validateCHTLJSSyntax(const CHTL::String& content) {
    // 检查基本的语法结构
    if (!validateBraceMatching(content)) {
        return false;
    }
    
    // 检查CHTL JS特定语法
    CHTL::StringVector blocks = extractCHTLJSBlocks();
    for (const auto& block : blocks) {
        if (block.find("{{") == CHTL::String::npos || block.find("}}") == CHTL::String::npos) {
            return false;
        }
    }
    
    return true;
}

CHTL::StringVector JSFileStream::findSyntaxErrors(const CHTL::String& content) {
    CHTL::StringVector errors;
    
    if (!validateBraceMatching(content)) {
        errors.push_back("括号不匹配");
    }
    
    // 检查未闭合的CHTL JS块
    size_t openCount = 0, closeCount = 0;
    size_t pos = 0;
    
    while ((pos = content.find("{{", pos)) != CHTL::String::npos) {
        openCount++;
        pos += 2;
    }
    
    pos = 0;
    while ((pos = content.find("}}", pos)) != CHTL::String::npos) {
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

CHTL::String JSFileStream::calculateMD5Hash(const CHTL::String& content) const {
    // 简化的哈希计算（在实际实现中应该使用真正的MD5）
    std::hash<CHTL::String> hasher;
    size_t hashValue = hasher(content);
    
    std::ostringstream oss;
    oss << std::hex << hashValue;
    return oss.str();
}

bool JSFileStream::validateBraceMatching(const CHTL::String& content) {
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
CHTLJSFileProcessor::FileType CHTLJSFileProcessor::detectFileType(const CHTL::String& filePath) {
    // Helper lambda for ends_with functionality (C++17 compatible)
    auto ends_with = [](const CHTL::String& str, const CHTL::String& suffix) {
        if (str.length() >= suffix.length()) {
            return str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
        }
        return false;
    };
    
    if (ends_with(filePath, ".chtljs") || ends_with(filePath, ".chtl")) {
        return FileType::CHTL_JS;
    } else if (ends_with(filePath, ".cjjs")) {
        return FileType::CJJS;
    } else if (ends_with(filePath, ".js")) {
        return FileType::PURE_JS;
    }
    return FileType::UNKNOWN;
}

CHTLJSFileProcessor::FileType CHTLJSFileProcessor::detectContentType(const CHTL::String& content) {
    if (containsCHTLJSMarkers(content)) {
        return FileType::CHTL_JS;
    } else if (containsModuleSyntax(content)) {
        return FileType::MODULE;
    } else {
        return FileType::PURE_JS;
    }
}

            CHTL::String  CHTLJSFileProcessor::preprocessCHTLJS(const CHTL::String& content) {
    CHTL::String processed = content;
    
    // 规范化行结束符
    processed = normalizeLineEndings(processed);
    
    // 其他预处理步骤...
    
    return processed;
}

            CHTL::String  CHTLJSFileProcessor::extractCHTLJSScript(const CHTL::String& content) {
    CHTL::String result;
    size_t pos = 0;
    
    while ((pos = content.find("{{", pos)) != CHTL::String::npos) {
        size_t endPos = content.find("}}", pos + 2);
        if (endPos != CHTL::String::npos) {
            CHTL::String block = content.substr(pos + 2, endPos - pos - 2);
            result += block + "\n";
            pos = endPos + 2;
        } else {
            break;
        }
    }
    
    return result;
}

CHTL::StringVector CHTLJSFileProcessor::findCJMODKeywords(const CHTL::String& content) {
    CHTL::StringVector keywords;
    
    // 查找常见的CJMOD关键字
    CHTL::StringVector searchKeywords = {"**", "await>>", "parallel>>", "timeout>>", "retry>>"};
    
    for (const auto& keyword : searchKeywords) {
        size_t pos = 0;
        while ((pos = content.find(keyword, pos)) != CHTL::String::npos) {
            keywords.push_back(keyword);
            pos += keyword.length();
        }
    }
    
    return keywords;
}

bool CHTLJSFileProcessor::containsCHTLJSMarkers(const CHTL::String& content) {
    return content.find("{{") != CHTL::String::npos && content.find("}}") != CHTL::String::npos;
}

bool CHTLJSFileProcessor::containsModuleSyntax(const CHTL::String& content) {
    return content.find("module") != CHTL::String::npos && content.find("load:") != CHTL::String::npos;
}

            CHTL::String  CHTLJSFileProcessor::normalizeLineEndings(const CHTL::String& content) {
    CHTL::String result;
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

CHTL::StringVector CHTLJSFileProcessor::splitIntoBlocks(const CHTL::String& content) {
    CHTL::StringVector blocks;
    
    std::istringstream iss(content);
    CHTL::String line;
    CHTL::String currentBlock;
    
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