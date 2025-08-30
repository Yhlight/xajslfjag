#include "NodeTest.h"
#include <chrono>
#include <memory>

namespace CHTL {
namespace Test {

// ========== NodeTest 实现 ==========

bool NodeTest::runAllTests() {
    std::cout << "=== Running Node Tests ===" << std::endl;
    
    bool allPassed = true;
    allPassed &= testBaseNode();
    allPassed &= testElementNode();
    allPassed &= testTextNode();
    allPassed &= testTemplateNode();
    allPassed &= testOperatorNode();
    allPassed &= testNodeFactory();
    allPassed &= testNodeCloning();
    allPassed &= testNodeValidation();
    allPassed &= testNodeSerialization();
    
    std::cout << "\n=== Node Tests " << (allPassed ? "PASSED" : "FAILED") << " ===" << std::endl;
    return allPassed;
}

bool NodeTest::testBaseNode() {
    std::cout << "\nTesting BaseNode..." << std::endl;
    
    bool success = true;
    
    // 测试节点创建
    auto node = std::make_shared<BaseNode>(CHTLNodeType::ELEMENT_NODE, "test");
    success &= assertTrue(node != nullptr, "Node creation");
    success &= assertEqual("test", node->getName(), "Node name");
    success &= assertTrue(node->getNodeType() == CHTLNodeType::ELEMENT_NODE, "Node type");
    
    // 测试属性管理
    node->setAttribute("class", "container");
    success &= assertTrue(node->hasAttribute("class"), "Has attribute");
    success &= assertEqual("container", node->getAttribute("class"), "Get attribute");
    
    // 测试子节点管理
    auto child = std::make_shared<BaseNode>(CHTLNodeType::TEXT_NODE, "child");
    node->addChild(child);
    success &= assertTrue(node->getChildCount() == 1, "Child count");
    success &= assertTrue(node->getChild(0) == child.get(), "Get child");
    
    printTestResult("BaseNode", success);
    return success;
}

bool NodeTest::testElementNode() {
    std::cout << "\nTesting ElementNode..." << std::endl;
    
    bool success = true;
    
    // 测试元素节点创建
    auto element = std::make_shared<ElementNode>("div");
    success &= assertTrue(element != nullptr, "Element creation");
    success &= assertEqual("div", element->getTagName(), "Tag name");
    success &= assertTrue(element->isBlockElement(), "Is block element");
    success &= assertFalse(element->isSelfClosing(), "Not self closing");
    
    // 测试CSS类管理
    element->addClass("container");
    element->addClass("active");
    success &= assertTrue(element->hasClass("container"), "Has class container");
    success &= assertTrue(element->hasClass("active"), "Has class active");
    
    auto classes = element->getClasses();
    success &= assertTrue(classes.size() == 2, "Class count");
    
    // 测试ID管理
    element->setId("main-content");
    success &= assertEqual("main-content", element->getId(), "Element ID");
    
    printTestResult("ElementNode", success);
    return success;
}

bool NodeTest::testTextNode() {
    std::cout << "\nTesting TextNode..." << std::endl;
    
    bool success = true;
    
    // 测试文本节点创建
    auto textNode = std::make_shared<TextNode>("Hello World");
    success &= assertTrue(textNode != nullptr, "Text node creation");
    success &= assertEqual("Hello World", textNode->getContent(), "Text content");
    
    // 测试文本类型
    textNode->setLiteral(true);
    success &= assertTrue(textNode->isLiteral(), "Is literal");
    success &= assertFalse(textNode->isDoubleQuoted(), "Not double quoted");
    
    // 测试文本处理
    textNode->setContent("   Hello   ");
    textNode->trim();
    success &= assertEqual("Hello", textNode->getContent(), "Trimmed text");
    
    // 测试空白检测
    auto whitespaceNode = std::make_shared<TextNode>("   \t\n  ");
    success &= assertTrue(whitespaceNode->isWhitespace(), "Is whitespace");
    
    printTestResult("TextNode", success);
    return success;
}

bool NodeTest::testTemplateNode() {
    std::cout << "\nTesting TemplateNode..." << std::endl;
    
    bool success = true;
    
    // 测试样式模板节点
    auto styleTemplate = std::make_shared<StyleTemplateNode>("ButtonStyle");
    success &= assertTrue(styleTemplate != nullptr, "Style template creation");
    success &= assertEqual("ButtonStyle", styleTemplate->getTemplateName(), "Template name");
    success &= assertTrue(styleTemplate->getTemplateType() == TemplateType::STYLE_TEMPLATE, "Template type");
    
    // 测试CSS属性管理
    styleTemplate->addCssProperty("color", "red");
    styleTemplate->addCssProperty("background", "blue");
    success &= assertTrue(styleTemplate->hasCssProperty("color"), "Has CSS property");
    success &= assertEqual("red", styleTemplate->getCssProperty("color"), "Get CSS property");
    
    // 测试继承
    styleTemplate->addInheritedTemplate("BaseStyle");
    success &= assertTrue(styleTemplate->inheritsFrom("BaseStyle"), "Inherits from BaseStyle");
    
    printTestResult("TemplateNode", success);
    return success;
}

bool NodeTest::testOperatorNode() {
    std::cout << "\nTesting OperatorNode..." << std::endl;
    
    bool success = true;
    
    // 测试use操作节点
    auto useNode = std::make_shared<UseNode>("html5");
    success &= assertTrue(useNode != nullptr, "Use node creation");
    success &= assertTrue(useNode->isHtml5Mode(), "Is HTML5 mode");
    success &= assertEqual("html5", useNode->getTarget(), "Use target");
    
    // 测试delete操作节点
    auto deleteNode = std::make_shared<DeleteNode>();
    deleteNode->addDeleteTarget("color");
    deleteNode->addDeleteTarget("background");
    success &= assertTrue(deleteNode->isDeleting("color"), "Is deleting color");
    success &= assertTrue(deleteNode->getDeleteTargets().size() == 2, "Delete targets count");
    
    // 测试insert操作节点
    auto insertNode = std::make_shared<InsertNode>();
    insertNode->setInsertPosition(InsertPosition::AFTER);
    insertNode->setTargetSelector("div[0]");
    insertNode->setIndex(0);
    success &= assertTrue(insertNode->getInsertPosition() == InsertPosition::AFTER, "Insert position");
    success &= assertTrue(insertNode->hasIndex(), "Has index");
    
    printTestResult("OperatorNode", success);
    return success;
}

bool NodeTest::testNodeFactory() {
    std::cout << "\nTesting NodeFactory..." << std::endl;
    
    bool success = true;
    
    // 测试节点工厂创建
    auto node = NodeFactory::createNode(CHTLNodeType::ELEMENT_NODE, "factory-test");
    success &= assertTrue(node != nullptr, "Factory node creation");
    success &= assertEqual("factory-test", node->getName(), "Factory node name");
    
    auto textNode = NodeFactory::createTextNode("Factory text");
    success &= assertTrue(textNode != nullptr, "Factory text node creation");
    success &= assertEqual("Factory text", textNode->getValue(), "Factory text content");
    
    auto elementNode = NodeFactory::createElementNode("span");
    success &= assertTrue(elementNode != nullptr, "Factory element node creation");
    success &= assertEqual("span", elementNode->getName(), "Factory element name");
    
    printTestResult("NodeFactory", success);
    return success;
}

bool NodeTest::testNodeCloning() {
    std::cout << "\nTesting Node Cloning..." << std::endl;
    
    bool success = true;
    
    // 测试浅克隆
    auto original = std::make_shared<ElementNode>("original");
    original->setId("original-id");
    original->addClass("original-class");
    
    auto cloned = std::static_pointer_cast<ElementNode>(original->clone());
    success &= assertTrue(cloned != nullptr, "Cloned node exists");
    success &= assertEqual(original->getTagName(), cloned->getTagName(), "Cloned tag name");
    success &= assertEqual(original->getId(), cloned->getId(), "Cloned ID");
    success &= assertTrue(cloned->hasClass("original-class"), "Cloned class");
    
    // 测试深度克隆
    auto child = std::make_shared<TextNode>("child text");
    original->addChild(child);
    
    auto deepCloned = std::static_pointer_cast<ElementNode>(original->deepClone());
    success &= assertTrue(deepCloned->getChildCount() == 1, "Deep cloned child count");
    success &= assertTrue(deepCloned->getChild(0) != original->getChild(0), "Deep cloned different child instance");
    
    printTestResult("Node Cloning", success);
    return success;
}

bool NodeTest::testNodeValidation() {
    std::cout << "\nTesting Node Validation..." << std::endl;
    
    bool success = true;
    
    ErrorReporter errorReporter;
    
    // 测试有效节点
    auto validElement = std::make_shared<ElementNode>("div");
    success &= assertTrue(validElement->validate(&errorReporter), "Valid element validation");
    
    // 测试无效节点
    auto invalidElement = std::make_shared<ElementNode>("");
    success &= assertFalse(invalidElement->validate(&errorReporter), "Invalid element validation");
    
    // 检查错误报告
    success &= assertTrue(errorReporter.hasErrors(), "Validation generated errors");
    
    printTestResult("Node Validation", success);
    return success;
}

bool NodeTest::testNodeSerialization() {
    std::cout << "\nTesting Node Serialization..." << std::endl;
    
    bool success = true;
    
    auto element = std::make_shared<ElementNode>("div");
    element->setId("test-id");
    element->addClass("test-class");
    
    // 测试toString
    std::string str = element->toString();
    success &= assertTrue(!str.empty(), "ToString not empty");
    success &= assertTrue(str.find("div") != std::string::npos, "ToString contains tag");
    
    // 测试toXml
    std::string xml = element->toXml();
    success &= assertTrue(!xml.empty(), "ToXml not empty");
    success &= assertTrue(xml.find("<") != std::string::npos, "ToXml contains XML tags");
    
    // 测试toJson
    std::string json = element->toJson();
    success &= assertTrue(!json.empty(), "ToJson not empty");
    success &= assertTrue(json.find("{") != std::string::npos, "ToJson contains JSON braces");
    
    printTestResult("Node Serialization", success);
    return success;
}

// ========== 辅助方法实现 ==========

bool NodeTest::assertTrue(bool condition, const std::string& message) {
    if (!condition) {
        std::cout << "  ❌ FAIL: " << message << std::endl;
        return false;
    }
    std::cout << "  ✓ PASS: " << message << std::endl;
    return true;
}

bool NodeTest::assertFalse(bool condition, const std::string& message) {
    return assertTrue(!condition, message);
}

bool NodeTest::assertEqual(const std::string& expected, const std::string& actual, const std::string& message) {
    if (expected != actual) {
        std::cout << "  ❌ FAIL: " << message << " (expected: '" << expected << "', actual: '" << actual << "')" << std::endl;
        return false;
    }
    std::cout << "  ✓ PASS: " << message << std::endl;
    return true;
}

bool NodeTest::assertNotNull(void* ptr, const std::string& message) {
    return assertTrue(ptr != nullptr, message);
}

void NodeTest::printTestResult(const std::string& testName, bool success) {
    std::cout << "  " << testName << ": " << (success ? "✓ PASSED" : "❌ FAILED") << std::endl;
}

// ========== ErrorReporterTest 实现 ==========

bool ErrorReporterTest::runAllTests() {
    std::cout << "\n=== Running ErrorReporter Tests ===" << std::endl;
    
    bool allPassed = true;
    allPassed &= testBasicErrorReporting();
    allPassed &= testErrorStatistics();
    allPassed &= testErrorFiltering();
    allPassed &= testErrorStateManagement();
    
    std::cout << "\n=== ErrorReporter Tests " << (allPassed ? "PASSED" : "FAILED") << " ===" << std::endl;
    return allPassed;
}

bool ErrorReporterTest::testBasicErrorReporting() {
    std::cout << "\nTesting Basic Error Reporting..." << std::endl;
    
    bool success = true;
    
    ErrorReporter reporter;
    ErrorPosition pos("test.chtl", 10, 5);
    
    reporter.info("Test info message", pos);
    reporter.warning("Test warning message", pos);
    reporter.error(ErrorType::SYNTAX_ERROR, "Test error message", pos);
    reporter.fatal(ErrorType::INTERNAL_ERROR, "Test fatal message", pos);
    
    success &= assertTrue(reporter.getErrorCount() == 4, "Error count");
    success &= assertTrue(reporter.hasErrors(), "Has errors");
    success &= assertTrue(reporter.hasFatalErrors(), "Has fatal errors");
    
    printTestResult("Basic Error Reporting", success);
    return success;
}

bool ErrorReporterTest::testErrorStatistics() {
    std::cout << "\nTesting Error Statistics..." << std::endl;
    
    bool success = true;
    
    ErrorReporter reporter;
    ErrorPosition pos("test.chtl", 1, 1);
    
    reporter.info("Info 1", pos);
    reporter.info("Info 2", pos);
    reporter.warning("Warning 1", pos);
    reporter.error(ErrorType::SYNTAX_ERROR, "Error 1", pos);
    reporter.fatal(ErrorType::INTERNAL_ERROR, "Fatal 1", pos);
    
    auto stats = reporter.getStatistics();
    success &= assertTrue(stats.infoCount == 2, "Info count");
    success &= assertTrue(stats.warningCount == 1, "Warning count");
    success &= assertTrue(stats.errorCount == 1, "Error count");
    success &= assertTrue(stats.fatalCount == 1, "Fatal count");
    success &= assertTrue(stats.totalCount() == 5, "Total count");
    
    printTestResult("Error Statistics", success);
    return success;
}

bool ErrorReporterTest::testErrorFiltering() {
    std::cout << "\nTesting Error Filtering..." << std::endl;
    
    bool success = true;
    
    ErrorReporter reporter;
    ErrorPosition pos("test.chtl", 1, 1);
    
    reporter.info("Info", pos);
    reporter.warning("Warning", pos);
    reporter.error(ErrorType::SYNTAX_ERROR, "Error", pos);
    
    auto infoErrors = reporter.getErrorsByLevel(ErrorLevel::INFO);
    auto syntaxErrors = reporter.getErrorsByType(ErrorType::SYNTAX_ERROR);
    
    success &= assertTrue(infoErrors.size() == 1, "Info filter count");
    success &= assertTrue(syntaxErrors.size() == 1, "Syntax error filter count");
    
    printTestResult("Error Filtering", success);
    return success;
}

bool ErrorReporterTest::testErrorStateManagement() {
    std::cout << "\nTesting Error State Management..." << std::endl;
    
    bool success = true;
    
    ErrorReporter reporter;
    ErrorPosition pos("test.chtl", 1, 1);
    
    reporter.error(ErrorType::SYNTAX_ERROR, "Error 1", pos);
    reporter.pushState();
    
    reporter.error(ErrorType::SEMANTIC_ERROR, "Error 2", pos);
    success &= assertTrue(reporter.getErrorCount() == 2, "Error count before pop");
    
    reporter.popState();
    success &= assertTrue(reporter.getErrorCount() == 1, "Error count after pop");
    
    printTestResult("Error State Management", success);
    return success;
}

bool ErrorReporterTest::assertTrue(bool condition, const std::string& message) {
    if (!condition) {
        std::cout << "  ❌ FAIL: " << message << std::endl;
        return false;
    }
    std::cout << "  ✓ PASS: " << message << std::endl;
    return true;
}

void ErrorReporterTest::printTestResult(const std::string& testName, bool success) {
    std::cout << "  " << testName << ": " << (success ? "✓ PASSED" : "❌ FAILED") << std::endl;
}

} // namespace Test
} // namespace CHTL