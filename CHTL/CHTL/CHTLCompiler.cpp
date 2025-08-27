#include "CHTLCompiler.h"
#include "CHTLContext.h"
#include "CHTLParser.h"
#include "CHTLGenerator.h"
#include <sstream>

namespace CHTL {

CHTLCompiler::CHTLCompiler() 
    : debugMode(false) {
    globalMap = std::make_shared<GlobalMap>();
    context = std::make_shared<CHTLContext>();
    parser = std::make_shared<CHTLParser>(globalMap);
    generator = std::make_shared<CHTLGenerator>(globalMap, context);
}

CHTLCompiler::~CHTLCompiler() = default;

std::string CHTLCompiler::Compile(const std::string& source, const std::string& context) {
    ClearErrors();
    
    try {
        // 设置调试模式
        parser->SetDebugMode(debugMode);
        
        // 解析源代码
        auto ast = parser->Parse(source);
        
        if (!ast) {
            ReportError("解析失败");
            return "";
        }
        
        // 检查解析错误
        if (parser->HasErrors()) {
            auto parserErrors = parser->GetErrors();
            errors.insert(errors.end(), parserErrors.begin(), parserErrors.end());
            return "";
        }
        
        // 生成代码
        std::string html = generator->Generate(ast);
        
        // 检查生成错误
        if (generator->HasErrors()) {
            auto genErrors = generator->GetErrors();
            errors.insert(errors.end(), genErrors.begin(), genErrors.end());
            return "";
        }
        
        // 组合最终输出
        std::stringstream output;
        
        // 获取全局样式
        std::string globalStyles = generator->GetGlobalStyles();
        if (!globalStyles.empty()) {
            output << "<style>\n" << globalStyles << "</style>\n";
        }
        
        // HTML内容
        output << html;
        
        // 获取全局脚本
        std::string globalScripts = generator->GetGlobalScripts();
        if (!globalScripts.empty()) {
            output << "<script>\n" << globalScripts << "</script>\n";
        }
        
        return output.str();
        
    } catch (const std::exception& e) {
        ReportError(std::string("编译异常: ") + e.what());
        return "";
    }
}

std::string CHTLCompiler::ProcessLocalStyle(const std::string& content, const std::string& elementContext) {
    // 创建一个临时的样式节点来处理局部样式
    std::stringstream styleSource;
    styleSource << "style {\n" << content << "\n}";
    
    try {
        auto styleAst = parser->Parse(styleSource.str());
        if (styleAst && styleAst->GetChildCount() > 0) {
            auto styleNode = styleAst->GetChild(0);
            
            // 设置上下文信息
            context->SetInLocalStyle(true);
            context->PushElement(elementContext);
            
            // 生成处理后的样式
            std::string result = generator->Generate(styleNode);
            
            // 恢复上下文
            context->PopElement();
            context->SetInLocalStyle(false);
            
            return result;
        }
    } catch (const std::exception& e) {
        ReportError(std::string("处理局部样式失败: ") + e.what());
    }
    
    return content;
}

void CHTLCompiler::AddModulePath(const std::string& path) {
    modulePaths.push_back(path);
    // TODO: 将模块路径传递给解析器和生成器
}

void CHTLCompiler::ReportError(const std::string& message) {
    errors.push_back(message);
}

} // namespace CHTL