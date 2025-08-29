#include "Parser.h"

namespace CHTL {

// 安全解析保护方法实现
bool Parser::checkParseDepth() {
    return currentDepth < maxParseDepth;
}

bool Parser::checkParseNodeCount() {
    return parseNodeCount < maxParseNodes;
}

void Parser::incrementDepth() {
    ++currentDepth;
}

void Parser::decrementDepth() {
    if (currentDepth > 0) {
        --currentDepth;
    }
}

void Parser::incrementNodeCount() {
    ++parseNodeCount;
}

} // namespace CHTL