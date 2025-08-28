#include "UseStatementParser.h"
#include <sstream>
#include <algorithm>

namespace CHTL {

UseStatementParser::UseStatementParser() {}

std::vector<UseStatementInfo> UseStatementParser::parse(const std::string& source, const std::string& sourceFile) {
	clearErrors();
	statistics_ = ParseStatistics();
	std::vector<UseStatementInfo> results;
	auto lines = splitIntoLines(source);
	for (size_t i = 0; i < lines.size(); ++i) {
		std::string line = trimWhitespace(lines[i]);
		if (isEmptyLine(line) || isCommentLine(line)) continue;
		if (!isUseStatement(line)) break; // use必须在文件开头，遇到非use即停止
		UseStatementInfo info = parseSingleUseStatement(line, i + 1, 1, sourceFile);
		if (!validateUseStatement(info)) {
			statistics_.parsingErrors++;
			continue;
		}
		results.push_back(info);
	}
	return results;
}

UseStatementInfo UseStatementParser::parseSingleUseStatement(const std::string& line, size_t lineNumber, size_t columnNumber, const std::string& sourceFile) {
	std::string text = trimWhitespace(line);
	UseStatementType t = getUseStatementType(text);
	switch (t) {
		case UseStatementType::HTML5:
			statistics_.html5Statements++;
			statistics_.totalUseStatements++;
			return parseHtml5UseStatement(text, lineNumber, columnNumber, sourceFile);
		case UseStatementType::CONFIG:
			statistics_.configStatements++;
			statistics_.totalUseStatements++;
			return parseConfigUseStatement(text, lineNumber, columnNumber, sourceFile);
		default:
			addError("Unknown use statement", lineNumber, columnNumber);
			return UseStatementInfo(UseStatementType::HTML5, "", lineNumber, columnNumber, sourceFile);
	}
}

bool UseStatementParser::validateUseStatement(const UseStatementInfo& useStatement) {
	if (useStatement.type == UseStatementType::HTML5) {
		return validateHtml5UseStatement(useStatement);
	}
	if (useStatement.type == UseStatementType::CONFIG) {
		return validateConfigUseStatement(useStatement);
	}
	return false;
}

std::vector<std::string> UseStatementParser::getValidationErrors(const UseStatementInfo& /*useStatement*/) {
	return errors_;
}

UseStatementType UseStatementParser::getUseStatementType(const std::string& text) {
	std::string norm = normalizeText(text);
	if (startsWith(norm, "use html5") && endsWith(norm, ";")) return UseStatementType::HTML5;
	if (startsWith(norm, "use @config ") && endsWith(norm, ";")) return UseStatementType::CONFIG;
	if (startsWith(norm, "use [configuration] @config ") && endsWith(norm, ";")) return UseStatementType::CONFIG;
	return UseStatementType::HTML5; // default, will validate later
}

bool UseStatementParser::isUseStatement(const std::string& text) {
	std::string norm = normalizeText(text);
	return startsWith(norm, "use html5") || startsWith(norm, "use @config ") || startsWith(norm, "use [configuration] @config ");
}

std::string UseStatementParser::extractUseStatementValue(const std::string& text, UseStatementType type) {
	std::string norm = normalizeText(text);
	if (type == UseStatementType::HTML5) return "html5";
	// use @Config Basic; or use [Configuration] @Config Basic;
	auto pos = norm.find("@config ");
	if (pos == std::string::npos) return "";
	std::string after = norm.substr(pos + 8); // len("@config ")
	if (!after.empty() && after.back() == ';') after.pop_back();
	after = trimWhitespace(after);
	return after;
}

std::shared_ptr<UseStatementNode> UseStatementParser::createUseStatementNode(const UseStatementInfo& info) {
	auto node = std::make_shared<UseStatementNode>(info.type == UseStatementType::HTML5 ? "html5" : "@Config", info.value, info.line, info.column);
	return node;
}

void UseStatementParser::clearErrors() { errors_.clear(); }
void UseStatementParser::clearStatistics() { statistics_ = ParseStatistics(); }
std::string UseStatementParser::getDebugInfo() const {
	std::ostringstream oss;
	oss << "use.total=" << statistics_.totalUseStatements
		<< ",html5=" << statistics_.html5Statements
		<< ",config=" << statistics_.configStatements
		<< ",errors=" << statistics_.parsingErrors;
	return oss.str();
}
void UseStatementParser::reset() { clearErrors(); clearStatistics(); }

// private helpers
std::vector<std::string> UseStatementParser::splitIntoLines(const std::string& source) {
	std::vector<std::string> lines;
	std::stringstream ss(source);
	std::string line;
	while (std::getline(ss, line)) lines.push_back(line);
	return lines;
}

std::string UseStatementParser::trimWhitespace(const std::string& text) {
	size_t start = 0; while (start < text.size() && isspace(static_cast<unsigned char>(text[start]))) ++start;
	size_t end = text.size(); while (end > start && isspace(static_cast<unsigned char>(text[end - 1]))) --end;
	return text.substr(start, end - start);
}

bool UseStatementParser::isCommentLine(const std::string& text) {
	std::string t = trimWhitespace(text);
	return startsWith(t, "//") || startsWith(t, "--");
}

bool UseStatementParser::isEmptyLine(const std::string& text) { return trimWhitespace(text).empty(); }

UseStatementInfo UseStatementParser::parseHtml5UseStatement(const std::string& text, size_t line, size_t column, const std::string& sourceFile) {
	// expect: use html5;
	std::string norm = normalizeText(text);
	if (!(startsWith(norm, "use html5") && endsWith(norm, ";"))) {
		addError("Invalid html5 use statement", line, column);
	}
	return UseStatementInfo(UseStatementType::HTML5, "html5", line, column, sourceFile);
}

UseStatementInfo UseStatementParser::parseConfigUseStatement(const std::string& text, size_t line, size_t column, const std::string& sourceFile) {
	std::string value = extractUseStatementValue(text, UseStatementType::CONFIG);
	if (value.empty()) addError("Missing config group name in use @Config", line, column);
	return UseStatementInfo(UseStatementType::CONFIG, value, line, column, sourceFile);
}

bool UseStatementParser::validateHtml5UseStatement(const UseStatementInfo& useStatement) {
	return useStatement.value == "html5";
}

bool UseStatementParser::validateConfigUseStatement(const UseStatementInfo& useStatement) {
	return !useStatement.value.empty();
}

void UseStatementParser::addError(const std::string& error, size_t /*line*/, size_t /*column*/) { errors_.push_back(error); }

void UseStatementParser::updateStatistics(const std::string& type, size_t value) {
	(void)type; (void)value;
}

std::string UseStatementParser::normalizeText(const std::string& text) {
	std::string s = text;
	std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return static_cast<char>(::tolower(c)); });
	return trimWhitespace(s);
}

bool UseStatementParser::startsWith(const std::string& text, const std::string& prefix) {
	return text.rfind(prefix, 0) == 0;
}

bool UseStatementParser::endsWith(const std::string& text, const std::string& suffix) {
	if (suffix.size() > text.size()) return false;
	return std::equal(suffix.rbegin(), suffix.rend(), text.rbegin());
}

std::string UseStatementParser::extractBetween(const std::string& text, const std::string& start, const std::string& end) {
	auto s = text.find(start);
	if (s == std::string::npos) return "";
	s += start.size();
	auto e = text.find(end, s);
	if (e == std::string::npos) return "";
	return trimWhitespace(text.substr(s, e - s));
}

} // namespace CHTL