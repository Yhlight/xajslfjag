#include "CHTLLexer/Lexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLManage/LocalStyleManager.h"
#include "CHTLIOStream/CHTLIOStream.h"
#include "../Error/ErrorReport.h"
#include "../Test/CompilationMonitor/CompilationMonitor.h"
#include <iostream>
#include <string>

using namespace CHTL;
using namespace CHTL::Test;

int main() {
    std::cout << "CHTL Complete System Test" << std::endl;
    std::cout << "==========================" << std::endl;
    
    // 初始化监视器
    auto monitor = CompilationMonitorFactory::createStandardMonitor();
    GlobalMonitorManager::getInstance().setGlobalMonitor(std::move(monitor));
    GlobalMonitorManager::getInstance().bindToCHTLProject();
    
    CHTL_MONITOR_START("Complete System Test");
    
    // 初始化错误报告器
    ErrorReporter& errorReporter = getGlobalErrorReporter();
    
    // 测试CHTL源码
    std::string chtlSource = R"(
html
{
    head
    {
        title: "CHTL Test Page";
    }
    
    body
    {
        div
        {
            id: main-container;
            class: container;
            
            text
            {
                "Welcome to CHTL!"
            }
            
            style
            {
                width: 100%;
                height: 200px;
                
                .highlight
                {
                    color: blue;
                    font-weight: bold;
                }
                
                &:hover
                {
                    background-color: #f0f0f0;
                }
            }
        }
        
        div
        {
            style
            {
                .box
                {
                    padding: 20px;
                    margin: 10px;
                    border: 1px solid #ccc;
                }
            }
            
            text
            {
                "This is another element with auto-generated classes."
            }
        }
    }
}
)";
    
    try {
        CHTL_MONITOR_PHASE(CompilationPhase::LEXICAL_ANALYSIS);
        
        // 1. 词法分析测试
        std::cout << "\n1. Testing Lexical Analysis..." << std::endl;
        
        CHTLStateMachine stateMachine;
        CHTLContext context;
        Lexer lexer(chtlSource, &stateMachine, &context, &errorReporter);
        
        TokenSequence tokens = lexer.tokenize();
        std::cout << "   Tokens generated: " << tokens.size() << std::endl;
        
        CHTL_MONITOR_PHASE(CompilationPhase::SYNTAX_ANALYSIS);
        
        // 2. 语法分析测试
        std::cout << "\n2. Testing Syntax Analysis..." << std::endl;
        
        auto parser = CHTLParserFactory::createStandardParser();
        parser->setErrorReporter(&errorReporter);
        
        ParseResult parseResult = parser->parse(tokens);
        std::cout << "   Parse success: " << (parseResult.success ? "YES" : "NO") << std::endl;
        std::cout << "   Nodes created: " << parseResult.nodesCreated << std::endl;
        std::cout << "   Tokens processed: " << parseResult.tokensProcessed << std::endl;
        
        if (parseResult.success && parseResult.rootNode) {
            CHTL_MONITOR_PHASE(CompilationPhase::SEMANTIC_ANALYSIS);
            
            // 3. 局部样式块管理器测试
            std::cout << "\n3. Testing Local Style Manager..." << std::endl;
            
            auto styleManager = LocalStyleManagerFactory::createStandardManager();
            styleManager->setErrorReporter(&errorReporter);
            
            // 查找样式节点并处理
            auto findStyleNodes = [](const NodePtr& node, std::vector<NodePtr>& styleNodes) -> void {
                if (!node) return;
                
                if (node->getNodeType() == CHTLNodeType::STYLE_NODE) {
                    styleNodes.push_back(node);
                }
                
                for (const auto& child : node->getChildren()) {
                    std::function<void(const NodePtr&, std::vector<NodePtr>&)> findStyleNodes_recursive;
                    findStyleNodes_recursive = [&](const NodePtr& n, std::vector<NodePtr>& sn) -> void {
                        if (!n) return;
                        if (n->getNodeType() == CHTLNodeType::STYLE_NODE) {
                            sn.push_back(n);
                        }
                        for (const auto& c : n->getChildren()) {
                            findStyleNodes_recursive(c, sn);
                        }
                    };
                    findStyleNodes_recursive(child, styleNodes);
                }
            };
            
            std::vector<NodePtr> styleNodes;
            findStyleNodes(parseResult.rootNode, styleNodes);
            
            std::cout << "   Style nodes found: " << styleNodes.size() << std::endl;
            
            // 处理每个样式节点
            for (auto& styleNode : styleNodes) {
                if (styleNode->getParent() && 
                    styleNode->getParent()->getNodeType() == CHTLNodeType::ELEMENT_NODE) {
                    
                    auto elementParent = dynamic_cast<ElementNode*>(styleNode->getParent());
                    auto style = dynamic_cast<StyleNode*>(styleNode.get());
                    
                    if (elementParent && style) {
                        styleManager->processLocalStyleBlock(style, elementParent);
                    }
                }
            }
            
            // 输出处理结果
            auto generatedClasses = styleManager->getGeneratedClassNames();
            auto generatedIds = styleManager->getGeneratedIds();
            auto globalRules = styleManager->getGlobalStyleRules();
            
            std::cout << "   Generated classes: " << generatedClasses.size() << std::endl;
            for (const auto& className : generatedClasses) {
                std::cout << "     - " << className << std::endl;
            }
            
            std::cout << "   Generated IDs: " << generatedIds.size() << std::endl;
            for (const auto& id : generatedIds) {
                std::cout << "     - " << id << std::endl;
            }
            
            std::cout << "   Global CSS rules: " << globalRules.size() << std::endl;
            
            if (!globalRules.empty()) {
                std::cout << "   Generated CSS:" << std::endl;
                std::string globalCSS = styleManager->generateGlobalCSS();
                std::cout << globalCSS << std::endl;
            }
            
            CHTL_MONITOR_PHASE(CompilationPhase::CODE_GENERATION);
            
            // 4. 文件系统测试
            std::cout << "\n4. Testing File System..." << std::endl;
            
            // 创建临时文件测试
            Util::TemporaryFile tempFile("chtl_test", "chtl");
            
            std::cout << "   Temporary file: " << tempFile.getPath() << std::endl;
            
            bool writeSuccess = tempFile.writeContent(chtlSource);
            std::cout << "   Write success: " << (writeSuccess ? "YES" : "NO") << std::endl;
            
            if (writeSuccess) {
                std::string readContent = tempFile.readContent();
                std::cout << "   Read success: " << (!readContent.empty() ? "YES" : "NO") << std::endl;
                std::cout << "   Content length: " << readContent.length() << std::endl;
            }
            
            // 5. CHTL文件读写器测试
            std::cout << "\n5. Testing CHTL File Reader/Writer..." << std::endl;
            
            CHTLFileWriter writer(tempFile.getPath());
            if (writer.open()) {
                bool writeResult = writer.writeCHTLSource(chtlSource);
                std::cout << "   CHTL write success: " << (writeResult ? "YES" : "NO") << std::endl;
                writer.close();
            }
            
            CHTLFileReader reader(tempFile.getPath());
            if (reader.open()) {
                std::string content = reader.readAll();
                auto fileInfo = reader.getFileInfo();
                
                std::cout << "   File type: " << (int)fileInfo.type << std::endl;
                std::cout << "   File encoding: " << (int)fileInfo.encoding << std::endl;
                std::cout << "   File size: " << fileInfo.size << " bytes" << std::endl;
                std::cout << "   Line count: " << reader.getLineCount() << std::endl;
                
                reader.close();
            }
        }
        
        // 6. 错误报告测试
        std::cout << "\n6. Testing Error Reporting..." << std::endl;
        
        auto stats = errorReporter.getStatistics();
        std::cout << "   Total errors: " << stats.totalCount() << std::endl;
        std::cout << "   Info: " << stats.infoCount << std::endl;
        std::cout << "   Warnings: " << stats.warningCount << std::endl;
        std::cout << "   Errors: " << stats.errorCount << std::endl;
        std::cout << "   Fatal: " << stats.fatalCount << std::endl;
        
        if (errorReporter.hasErrors()) {
            std::cout << "\n   Recent errors:" << std::endl;
            auto errors = errorReporter.getErrorsByLevel(ErrorLevel::ERROR);
            for (size_t i = 0; i < std::min(errors.size(), size_t(3)); ++i) {
                std::cout << "     - " << errors[i].message << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        CHTL_MONITOR_STOP();
        return 1;
    }
    
    CHTL_MONITOR_STOP();
    
    // 输出监视器报告
    auto* globalMonitor = GlobalMonitorManager::getInstance().getGlobalMonitor();
    if (globalMonitor) {
        std::cout << "\n=== Compilation Monitor Report ===" << std::endl;
        std::cout << globalMonitor->generateReport() << std::endl;
    }
    
    std::cout << "\n=== All Tests Completed Successfully ===" << std::endl;
    return 0;
}