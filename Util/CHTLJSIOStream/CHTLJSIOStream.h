#ifndef CHTLJSIOSTREAM_H
#define CHTLJSIOSTREAM_H

#include <string>
#include <vector>
#include <memory>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <map>

namespace CHTLJS {

/**
 * @brief CHTL JS I/O流基类
 * 提供CHTL JS语法的文件读写、流操作、编码转换等功能
 * 与CHTL完全独立，不共享任何组件
 */
class CHTLJSIOStream {
public:
    /**
     * @brief 流模式枚举
     */
    enum class StreamMode {
        READ,           // 只读模式
        WRITE,          // 只写模式
        READ_WRITE,     // 读写模式
        APPEND          // 追加模式
    };

    /**
     * @brief 编码类型枚举
     */
    enum class Encoding {
        UTF8,           // UTF-8编码
        UTF16,          // UTF-16编码
        UTF32,          // UTF-32编码
        GBK,            // GBK编码
        ISO8859_1       // ISO-8859-1编码
    };

    /**
     * @brief 构造函数
     * @param mode 流模式
     * @param encoding 编码类型
     */
    explicit CHTLJSIOStream(StreamMode mode = StreamMode::READ, Encoding encoding = Encoding::UTF8);
    
    /**
     * @brief 虚析构函数
     */
    virtual ~CHTLJSIOStream();

    /**
     * @brief 打开文件
     * @param filePath 文件路径
     * @return 是否成功打开
     */
    virtual bool open(const std::string& filePath);
    
    /**
     * @brief 关闭文件
     */
    virtual void close();
    
    /**
     * @brief 检查文件是否打开
     * @return 是否打开
     */
    virtual bool isOpen() const;
    
    /**
     * @brief 读取CHTL JS内容
     * @return CHTL JS内容字符串
     */
    virtual std::string readCHTLJS();
    
    /**
     * @brief 写入CHTL JS内容
     * @param content CHTL JS内容
     * @return 是否成功写入
     */
    virtual bool writeCHTLJS(const std::string& content);
    
    /**
     * @brief 读取指定行数
     * @param startLine 起始行号
     * @param endLine 结束行号
     * @return 指定行范围的内容
     */
    virtual std::string readLines(size_t startLine, size_t endLine);
    
    /**
     * @brief 写入指定行
     * @param lineNumber 行号
     * @param content 内容
     * @return 是否成功写入
     */
    virtual bool writeLine(size_t lineNumber, const std::string& content);
    
    /**
     * @brief 插入内容到指定行
     * @param lineNumber 行号
     * @param content 内容
     * @return 是否成功插入
     */
    virtual bool insertLine(size_t lineNumber, const std::string& content);
    
    /**
     * @brief 删除指定行
     * @param lineNumber 行号
     * @return 是否成功删除
     */
    virtual bool deleteLine(size_t lineNumber);
    
    /**
     * @brief 获取文件总行数
     * @return 总行数
     */
    virtual size_t getLineCount() const;
    
    /**
     * @brief 获取当前行号
     * @return 当前行号
     */
    virtual size_t getCurrentLine() const;
    
    /**
     * @brief 设置编码
     * @param encoding 编码类型
     */
    virtual void setEncoding(Encoding encoding);
    
    /**
     * @brief 获取编码
     * @return 编码类型
     */
    virtual Encoding getEncoding() const;
    
    /**
     * @brief 编码转换
     * @param content 源内容
     * @param fromEncoding 源编码
     * @param toEncoding 目标编码
     * @return 转换后的内容
     */
    static std::string convertEncoding(const std::string& content, Encoding fromEncoding, Encoding toEncoding);
    
    /**
     * @brief 获取错误信息
     * @return 错误信息
     */
    virtual std::string getLastError() const;
    
    /**
     * @brief 清空错误信息
     */
    virtual void clearError();

protected:
    StreamMode mode_;                    // 流模式
    Encoding encoding_;                  // 编码类型
    std::string filePath_;              // 文件路径
    std::fstream fileStream_;           // 文件流
    std::vector<std::string> lines_;    // 文件行缓存
    size_t currentLine_;                // 当前行号
    std::string lastError_;             // 最后错误信息
    bool isOpen_;                       // 是否打开状态
    
    /**
     * @brief 加载文件内容到内存
     * @return 是否成功加载
     */
    virtual bool loadFileContent();
    
    /**
     * @brief 保存文件内容到磁盘
     * @return 是否成功保存
     */
    virtual bool saveFileContent();
    
    /**
     * @brief 更新行缓存
     */
    virtual void updateLineCache();
    
    /**
     * @brief 设置错误信息
     * @param error 错误信息
     */
    virtual void setError(const std::string& error);
};

/**
 * @brief CHTL JS模块I/O流
 * 专门用于处理CHTL JS模块文件
 */
class CHTLJSModuleIOStream : public CHTLJSIOStream {
public:
    /**
     * @brief 构造函数
     * @param mode 流模式
     * @param encoding 编码类型
     */
    explicit CHTLJSModuleIOStream(StreamMode mode = StreamMode::READ, Encoding encoding = Encoding::UTF8);
    
    /**
     * @brief 读取模块内容
     * @return 模块内容
     */
    virtual std::string readModule();
    
    /**
     * @brief 写入模块内容
     * @param moduleContent 模块内容
     * @return 是否成功写入
     */
    virtual bool writeModule(const std::string& moduleContent);
    
    /**
     * @brief 获取模块名称
     * @return 模块名称
     */
    virtual std::string getModuleName() const;
    
    /**
     * @brief 设置模块名称
     * @param name 模块名称
     */
    virtual void setModuleName(const std::string& name);
    
    /**
     * @brief 获取模块依赖
     * @return 依赖列表
     */
    virtual std::vector<std::string> getDependencies() const;
    
    /**
     * @brief 添加模块依赖
     * @param dependency 依赖名称
     */
    virtual void addDependency(const std::string& dependency);

private:
    std::string moduleName_;            // 模块名称
    std::vector<std::string> dependencies_; // 依赖列表
};

/**
 * @brief CHTL JS配置I/O流
 * 专门用于处理CHTL JS配置文件
 */
class CHTLJSConfigIOStream : public CHTLJSIOStream {
public:
    /**
     * @brief 构造函数
     * @param mode 流模式
     * @param encoding 编码类型
     */
    explicit CHTLJSConfigIOStream(StreamMode mode = StreamMode::READ, Encoding encoding = Encoding::UTF8);
    
    /**
     * @brief 读取配置项
     * @param key 配置键
     * @return 配置值
     */
    virtual std::string readConfig(const std::string& key);
    
    /**
     * @brief 写入配置项
     * @param key 配置键
     * @param value 配置值
     * @return 是否成功写入
     */
    virtual bool writeConfig(const std::string& key, const std::string& value);
    
    /**
     * @brief 删除配置项
     * @param key 配置键
     * @return 是否成功删除
     */
    virtual bool deleteConfig(const std::string& key);
    
    /**
     * @brief 获取所有配置键
     * @return 配置键列表
     */
    virtual std::vector<std::string> getAllConfigKeys() const;

private:
    std::map<std::string, std::string> configMap_;  // 配置映射表
};

} // namespace CHTLJS

#endif // CHTLJSIOSTREAM_H