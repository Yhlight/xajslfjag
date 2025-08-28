#pragma once

#include "CHTLJSToken.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTLJS {

class CHTLJSLexer {
public:
	CHTLJSLexer();
	~CHTLJSLexer() = default;

	void setSource(const std::string& source);
	std::shared_ptr<CHTLJSToken> getNextToken();
	std::vector<std::shared_ptr<CHTLJSToken>> getAllTokens();
	void reset();

	bool isEOF() const { return currentPos_ >= source_.size(); }
	size_t getLine() const { return currentLine_; }
	size_t getColumn() const { return currentColumn_; }

private:
	std::string source_;
	size_t currentPos_;
	size_t currentLine_;
	size_t currentColumn_;

	void skipWhitespace();
	char peek(size_t offset = 0) const;
	void advance(size_t n = 1);
	void advanceLine();

	std::shared_ptr<CHTLJSToken> readIdentifierOrKeyword();
	std::shared_ptr<CHTLJSToken> readString();
	std::shared_ptr<CHTLJSToken> readNumber();
	std::shared_ptr<CHTLJSToken> readEnhancedSelector(); // {{...}}

	CHTLJSTokenType keywordOf(const std::string& id) const;
};

} // namespace CHTLJS