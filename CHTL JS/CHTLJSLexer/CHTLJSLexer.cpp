#include "CHTLJSLexer.h"
#include <cctype>

namespace CHTLJS {

CHTLJSLexer::CHTLJSLexer()
	: currentPos_(0), currentLine_(1), currentColumn_(1) {}

void CHTLJSLexer::setSource(const std::string& source) {
	source_ = source;
	reset();
}

std::shared_ptr<CHTLJSToken> CHTLJSLexer::getNextToken() {
	while (!isEOF()) {
		skipWhitespace();
		if (isEOF()) break;
		char c = peek();
		if (std::isalpha(static_cast<unsigned char>(c)) || c == '_') {
			return readIdentifierOrKeyword();
		}
		if (c == '"' || c == '\'') {
			return readString();
		}
		if (std::isdigit(static_cast<unsigned char>(c))) {
			return readNumber();
		}
		if (c == '{' && peek(1) == '{') {
			return readEnhancedSelector();
		}
		// 分隔符与操作符占位
		switch (c) {
			case '{': advance(); return createToken(CHTLJSTokenType::LEFT_BRACE, "{", currentLine_, currentColumn_-1);
			case '}': advance(); return createToken(CHTLJSTokenType::RIGHT_BRACE, "}", currentLine_, currentColumn_-1);
			case '[': advance(); return createToken(CHTLJSTokenType::LEFT_BRACKET, "[", currentLine_, currentColumn_-1);
			case ']': advance(); return createToken(CHTLJSTokenType::RIGHT_BRACKET, "]", currentLine_, currentColumn_-1);
			case '(': advance(); return createToken(CHTLJSTokenType::LEFT_PAREN, "(", currentLine_, currentColumn_-1);
			case ')': advance(); return createToken(CHTLJSTokenType::RIGHT_PAREN, ")", currentLine_, currentColumn_-1);
			case ';': advance(); return createToken(CHTLJSTokenType::SEMICOLON, ";", currentLine_, currentColumn_-1);
			case ':': advance(); return createToken(CHTLJSTokenType::COLON, ":", currentLine_, currentColumn_-1);
			case '=': advance(); return createToken(CHTLJSTokenType::EQUAL, "=", currentLine_, currentColumn_-1);
			case ',': advance(); return createToken(CHTLJSTokenType::COMMA, ",", currentLine_, currentColumn_-1);
			case '.': advance(); return createToken(CHTLJSTokenType::DOT, ".", currentLine_, currentColumn_-1);
			default: break;
		}
		// 未识别字符，跳过为UNKNOWN
		char u = peek();
		advance();
		return createToken(CHTLJSTokenType::UNKNOWN, std::string(1, u), currentLine_, currentColumn_-1);
	}
	return createToken(CHTLJSTokenType::END_OF_FILE, "", currentLine_, currentColumn_);
}

std::vector<std::shared_ptr<CHTLJSToken>> CHTLJSLexer::getAllTokens() {
	std::vector<std::shared_ptr<CHTLJSToken>> r;
	reset();
	while (true) {
		auto t = getNextToken();
		r.push_back(t);
		if (t->getType() == CHTLJSTokenType::END_OF_FILE) break;
	}
	return r;
}

void CHTLJSLexer::reset() {
	currentPos_ = 0;
	currentLine_ = 1;
	currentColumn_ = 1;
}

void CHTLJSLexer::skipWhitespace() {
	while (!isEOF() && std::isspace(static_cast<unsigned char>(peek()))) {
		if (peek() == '\n') {
			advanceLine();
		} else {
			advance();
		}
	}
}

char CHTLJSLexer::peek(size_t offset) const {
	size_t pos = currentPos_ + offset;
	if (pos >= source_.size()) return '\0';
	return source_[pos];
}

void CHTLJSLexer::advance(size_t n) {
	currentPos_ += n;
	currentColumn_ += n;
}

void CHTLJSLexer::advanceLine() {
	currentPos_++;
	currentLine_++;
	currentColumn_ = 1;
}

std::shared_ptr<CHTLJSToken> CHTLJSLexer::readIdentifierOrKeyword() {
	size_t startCol = currentColumn_;
	std::string id;
	while (!isEOF() && (std::isalnum(static_cast<unsigned char>(peek())) || peek() == '_')) {
		id.push_back(peek());
		advance();
	}
	auto kt = keywordOf(id);
	if (kt != CHTLJSTokenType::IDENTIFIER) {
		return createToken(kt, id, currentLine_, startCol);
	}
	return createToken(CHTLJSTokenType::IDENTIFIER, id, currentLine_, startCol);
}

std::shared_ptr<CHTLJSToken> CHTLJSLexer::readString() {
	char q = peek();
	size_t startCol = currentColumn_;
	advance();
	std::string v;
	while (!isEOF() && peek() != q) {
		if (peek() == '\\') {
			advance();
			if (!isEOF()) { v.push_back(peek()); advance(); }
		} else { v.push_back(peek()); advance(); }
	}
	if (!isEOF()) advance();
	return createToken(CHTLJSTokenType::STRING_LITERAL, v, currentLine_, startCol);
}

std::shared_ptr<CHTLJSToken> CHTLJSLexer::readNumber() {
	size_t startCol = currentColumn_;
	std::string v;
	while (!isEOF() && std::isdigit(static_cast<unsigned char>(peek()))) { v.push_back(peek()); advance(); }
	if (!isEOF() && peek() == '.') { v.push_back('.'); advance();
		while (!isEOF() && std::isdigit(static_cast<unsigned char>(peek()))) { v.push_back(peek()); advance(); }
	}
	return createToken(CHTLJSTokenType::NUMBER, v, currentLine_, startCol);
}

std::shared_ptr<CHTLJSToken> CHTLJSLexer::readEnhancedSelector() {
	size_t startCol = currentColumn_;
	// consume {{
	advance(2);
	std::string buf;
	while (!isEOF()) {
		if (peek() == '}' && peek(1) == '}') { advance(2); break; }
		buf.push_back(peek());
		advance();
	}
	return createToken(CHTLJSTokenType::ENHANCED_SELECTOR, buf, currentLine_, startCol);
}

CHTLJSTokenType CHTLJSLexer::keywordOf(const std::string& id) const {
	if (id == "vir") return CHTLJSTokenType::KEYWORD_VIR;
	if (id == "listen") return CHTLJSTokenType::KEYWORD_LISTEN;
	if (id == "delegate") return CHTLJSTokenType::KEYWORD_DELEGATE;
	if (id == "animate") return CHTLJSTokenType::KEYWORD_ANIMATE;
	if (id == "target") return CHTLJSTokenType::KEYWORD_TARGET;
	if (id == "duration") return CHTLJSTokenType::KEYWORD_DURATION;
	if (id == "easing") return CHTLJSTokenType::KEYWORD_EASING;
	if (id == "begin") return CHTLJSTokenType::KEYWORD_BEGIN;
	if (id == "when") return CHTLJSTokenType::KEYWORD_WHEN;
	if (id == "end") return CHTLJSTokenType::KEYWORD_END;
	if (id == "loop") return CHTLJSTokenType::KEYWORD_LOOP;
	if (id == "direction") return CHTLJSTokenType::KEYWORD_DIRECTION;
	if (id == "delay") return CHTLJSTokenType::KEYWORD_DELAY;
	if (id == "callback") return CHTLJSTokenType::KEYWORD_CALLBACK;
	if (id == "at") return CHTLJSTokenType::KEYWORD_AT;
	return CHTLJSTokenType::IDENTIFIER;
}

} // namespace CHTLJS