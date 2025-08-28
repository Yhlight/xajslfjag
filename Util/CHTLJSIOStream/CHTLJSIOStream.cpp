#include "CHTLJSIOStream.h"
#include <algorithm>
#include <cctype>
#include <locale>
#include <codecvt>
#include <stdexcept>

namespace CHTLJS {

// CHTLJSIOStream 实现
CHTLJSIOStream::CHTLJSIOStream(StreamMode mode, Encoding encoding)
    : mode_(mode), encoding_(encoding), currentLine_(0), isOpen_(false) {
}

CHTLJSIOStream::~CHTLJSIOStream() {
    if (isOpen_) {
        close();
    }
}

bool CHTLJSIOStream::open(const std::string& filePath) {
    if (isOpen_) {
        close();
    }
    
    filePath_ = filePath;
    
    std::ios::openmode openMode = std::ios::binary;
    switch (mode_) {
        case StreamMode::READ:
            openMode |= std::ios::in;
            break;
        case StreamMode::WRITE:
            openMode |= std::ios::out | std::ios::trunc;
            break;
        case StreamMode::READ_WRITE:
            openMode |= std::ios::in | std::ios::out;
            break;
        case StreamMode::APPEND:
            openMode |= std::ios::out | std::ios::app;
            break;
    }
    
    fileStream_.open(filePath_, openMode);
    if (!fileStream_.is_open()) {
        setError("无法打开文件: " + filePath_);
        return false;
    }
    
    isOpen_ = true;
    currentLine_ = 0;
    
    if (mode_ == StreamMode::READ || mode_ == StreamMode::READ_WRITE) {
        if (!loadFileContent()) {
            setError("无法加载文件内容");
            return false;
        }
    }
    
    return true;
}

void CHTLJSIOStream::close() {
    if (isOpen_) {
        if (mode_ == StreamMode::WRITE || mode_ == StreamMode::READ_WRITE || mode_ == StreamMode::APPEND) {
            saveFileContent();
        }
        fileStream_.close();
        isOpen_ = false;
        lines_.clear();
        currentLine_ = 0;
    }
}

bool CHTLJSIOStream::isOpen() const {
    return isOpen_;
}

std::string CHTLJSIOStream::readCHTLJS() {
    if (!isOpen_ || (mode_ != StreamMode::READ && mode_ != StreamMode::READ_WRITE)) {
        setError("流未打开或不是读模式");
        return "";
    }
    
    std::string result;
    for (const auto& line : lines_) {
        result += line + "\n";
    }
    
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    
    return result;
}

bool CHTLJSIOStream::writeCHTLJS(const std::string& content) {
    if (!isOpen_ || (mode_ != StreamMode::WRITE && mode_ != StreamMode::READ_WRITE && mode_ != StreamMode::APPEND)) {
        setError("流未打开或不是写模式");
        return false;
    }
    
    // 清空现有内容
    if (mode_ == StreamMode::WRITE) {
        lines_.clear();
    }
    
    // 按行分割内容
    std::istringstream iss(content);
    std::string line;
    while (std::getline(iss, line)) {
        lines_.push_back(line);
    }
    
    currentLine_ = lines_.size();
    
    if (!saveFileContent()) {
        setError("无法保存文件内容");
        return false;
    }
    
    return true;
}

std::string CHTLJSIOStream::readLines(size_t startLine, size_t endLine) {
    if (!isOpen_ || (mode_ != StreamMode::READ && mode_ != StreamMode::READ_WRITE)) {
        setError("流未打开或不是读模式");
        return "";
    }
    
    if (startLine >= lines_.size() || endLine >= lines_.size() || startLine > endLine) {
        setError("行号超出范围");
        return "";
    }
    
    std::string result;
    for (size_t i = startLine; i <= endLine; ++i) {
        result += lines_[i] + "\n";
    }
    
    if (!result.empty() && result.back() == '\n') {
        result.pop_back();
    }
    
    return result;
}

bool CHTLJSIOStream::writeLine(size_t lineNumber, const std::string& content) {
    if (!isOpen_ || (mode_ != StreamMode::WRITE && mode_ != StreamMode::READ_WRITE)) {
        setError("流未打开或不是写模式");
        return false;
    }
    
    if (lineNumber >= lines_.size()) {
        // 扩展行数
        while (lines_.size() <= lineNumber) {
            lines_.push_back("");
        }
    }
    
    lines_[lineNumber] = content;
    currentLine_ = lineNumber;
    
    if (!saveFileContent()) {
        setError("无法保存文件内容");
        return false;
    }
    
    return true;
}

bool CHTLJSIOStream::insertLine(size_t lineNumber, const std::string& content) {
    if (!isOpen_ || (mode_ != StreamMode::WRITE && mode_ != StreamMode::READ_WRITE)) {
        setError("流未打开或不是写模式");
        return false;
    }
    
    if (lineNumber > lines_.size()) {
        lineNumber = lines_.size();
    }
    
    lines_.insert(lines_.begin() + lineNumber, content);
    currentLine_ = lineNumber;
    
    if (!saveFileContent()) {
        setError("无法保存文件内容");
        return false;
    }
    
    return true;
}

bool CHTLJSIOStream::deleteLine(size_t lineNumber) {
    if (!isOpen_ || (mode_ != StreamMode::WRITE && mode_ != StreamMode::READ_WRITE)) {
        setError("流未打开或不是写模式");
        return false;
    }
    
    if (lineNumber >= lines_.size()) {
        setError("行号超出范围");
        return false;
    }
    
    lines_.erase(lines_.begin() + lineNumber);
    
    if (currentLine_ >= lineNumber) {
        if (currentLine_ > 0) {
            currentLine_--;
        }
    }
    
    if (!saveFileContent()) {
        setError("无法保存文件内容");
        return false;
    }
    
    return true;
}

size_t CHTLJSIOStream::getLineCount() const {
    return lines_.size();
}

size_t CHTLJSIOStream::getCurrentLine() const {
    return currentLine_;
}

void CHTLJSIOStream::setEncoding(Encoding encoding) {
    encoding_ = encoding;
}

CHTLJSIOStream::Encoding CHTLJSIOStream::getEncoding() const {
    return encoding_;
}

std::string CHTLJSIOStream::convertEncoding(const std::string& content, Encoding fromEncoding, Encoding toEncoding) {
    if (fromEncoding == toEncoding) {
        return content;
    }
    
    try {
        // 这里实现编码转换逻辑
        // 由于C++标准库的编码转换支持有限，这里提供基本实现
        if (fromEncoding == Encoding::UTF8 && toEncoding == Encoding::UTF16) {
            std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
            std::u16string utf16 = converter.from_bytes(content);
            return std::string(reinterpret_cast<const char*>(utf16.data()), utf16.size() * 2);
        } else if (fromEncoding == Encoding::UTF16 && toEncoding == Encoding::UTF8) {
            std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
            std::u16string utf16(reinterpret_cast<const char16_t*>(content.data()), content.size() / 2);
            return converter.to_bytes(utf16);
        }
        
        // 其他编码转换可以在这里扩展
        return content;
    } catch (const std::exception& e) {
        return content; // 转换失败时返回原内容
    }
}

std::string CHTLJSIOStream::getLastError() const {
    return lastError_;
}

void CHTLJSIOStream::clearError() {
    lastError_.clear();
}

bool CHTLJSIOStream::loadFileContent() {
    if (!fileStream_.is_open()) {
        return false;
    }
    
    lines_.clear();
    fileStream_.seekg(0);
    
    std::string line;
    while (std::getline(fileStream_, line)) {
        // 移除Windows风格的\r
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        lines_.push_back(line);
    }
    
    currentLine_ = 0;
    return true;
}

bool CHTLJSIOStream::saveFileContent() {
    if (!fileStream_.is_open()) {
        return false;
    }
    
    fileStream_.seekp(0);
    // 对于fstream，我们需要重新打开文件来截断
    fileStream_.close();
    fileStream_.open(filePath_, std::ios::out | std::ios::trunc);
    
    for (size_t i = 0; i < lines_.size(); ++i) {
        fileStream_ << lines_[i];
        if (i < lines_.size() - 1) {
            fileStream_ << '\n';
        }
    }
    
    fileStream_.flush();
    return fileStream_.good();
}

void CHTLJSIOStream::updateLineCache() {
    // 重新加载文件内容到缓存
    if (isOpen_) {
        loadFileContent();
    }
}

void CHTLJSIOStream::setError(const std::string& error) {
    lastError_ = error;
}

// CHTLJSModuleIOStream 实现
CHTLJSModuleIOStream::CHTLJSModuleIOStream(StreamMode mode, Encoding encoding)
    : CHTLJSIOStream(mode, encoding) {
}

std::string CHTLJSModuleIOStream::readModule() {
    return readCHTLJS();
}

bool CHTLJSModuleIOStream::writeModule(const std::string& moduleContent) {
    return writeCHTLJS(moduleContent);
}

std::string CHTLJSModuleIOStream::getModuleName() const {
    return moduleName_;
}

void CHTLJSModuleIOStream::setModuleName(const std::string& name) {
    moduleName_ = name;
}

std::vector<std::string> CHTLJSModuleIOStream::getDependencies() const {
    return dependencies_;
}

void CHTLJSModuleIOStream::addDependency(const std::string& dependency) {
    // 检查是否已存在
    if (std::find(dependencies_.begin(), dependencies_.end(), dependency) == dependencies_.end()) {
        dependencies_.push_back(dependency);
    }
}

// CHTLJSConfigIOStream 实现
CHTLJSConfigIOStream::CHTLJSConfigIOStream(StreamMode mode, Encoding encoding)
    : CHTLJSIOStream(mode, encoding) {
}

std::string CHTLJSConfigIOStream::readConfig(const std::string& key) {
    auto it = configMap_.find(key);
    return (it != configMap_.end()) ? it->second : "";
}

bool CHTLJSConfigIOStream::writeConfig(const std::string& key, const std::string& value) {
    configMap_[key] = value;
    
    // 将配置写入文件
    std::string configContent;
    for (const auto& pair : configMap_) {
        configContent += pair.first + "=" + pair.second + "\n";
    }
    
    return writeCHTLJS(configContent);
}

bool CHTLJSConfigIOStream::deleteConfig(const std::string& key) {
    auto it = configMap_.find(key);
    if (it != configMap_.end()) {
        configMap_.erase(it);
        
        // 重新写入文件
        std::string configContent;
        for (const auto& pair : configMap_) {
            configContent += pair.first + "=" + pair.second + "\n";
        }
        
        return writeCHTLJS(configContent);
    }
    return false;
}

std::vector<std::string> CHTLJSConfigIOStream::getAllConfigKeys() const {
    std::vector<std::string> keys;
    keys.reserve(configMap_.size());
    for (const auto& pair : configMap_) {
        keys.push_back(pair.first);
    }
    return keys;
}

} // namespace CHTLJS