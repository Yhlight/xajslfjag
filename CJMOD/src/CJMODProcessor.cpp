#include "CJMOD/CJMODProcessor.h"
#include <algorithm>
#include <cctype>
#include <sstream>

namespace CHTL {

CJMODProcessor::CJMODProcessor() {
}

std::vector<CodeSlice> CJMODProcessor::Process(const CodeSlice& slice) {
    // Only process CHTL_JS slices
    if (slice.Type != SliceType::CHTL_JS) {
        return {slice};
    }
    
    std::string processedContent = slice.Content;
    auto scanResults = DualPointerScan(slice.Content);
    
    // Process from end to beginning to maintain positions
    for (auto it = scanResults.rbegin(); it != scanResults.rend(); ++it) {
        const auto& result = *it;
        
        // Check if this keyword has a registered handler
        auto patternInfo = CJMODPatternRegistry::GetInstance().GetPatternInfo(result.Keyword);
        if (patternInfo) {
            // Apply prefix capture if needed
            std::string prefix = CapturePrefix(slice.Content, result.Start, result.Keyword);
            
            // Generate replacement code
            std::string replacement = patternInfo->Handler(result.Arguments);
            
            // If prefix was captured, prepend it to arguments
            if (!prefix.empty()) {
                replacement = prefix + replacement;
            }
            
            // Replace in content
            processedContent.replace(result.Start, result.End - result.Start, replacement);
        }
    }
    
    CodeSlice processedSlice = slice;
    processedSlice.Content = processedContent;
    // After processing CJMOD patterns, the output is JavaScript
    processedSlice.Type = SliceType::JS;
    
    return {processedSlice};
}

std::vector<CJMODProcessor::ScanResult> CJMODProcessor::DualPointerScan(const std::string& content) {
    std::vector<ScanResult> results;
    
    size_t front = 0;
    size_t back = 0;
    
    while (front < content.length()) {
        // Skip whitespace and non-keyword characters
        while (front < content.length() && !std::isalpha(content[front])) {
            front++;
            back = front;
        }
        
        if (front >= content.length()) break;
        
        // Scan potential keyword
        size_t keywordStart = front;
        while (front < content.length() && std::isalnum(content[front])) {
            front++;
        }
        
        std::string potentialKeyword = content.substr(keywordStart, front - keywordStart);
        
        // Check if it's a registered CJMOD keyword
        auto patternInfo = CJMODPatternRegistry::GetInstance().GetPatternInfo(potentialKeyword);
        if (patternInfo) {
            // Found a keyword, now collect arguments
            ScanResult result;
            result.Keyword = potentialKeyword;
            result.Start = keywordStart;
            
            // Skip whitespace after keyword
            while (front < content.length() && std::isspace(content[front])) {
                front++;
            }
            
            // Collect arguments based on pattern
            if (front < content.length() && content[front] == '(') {
                // Function-style arguments
                front++; // Skip '('
                int parenCount = 1;
                size_t argStart = front;
                
                while (front < content.length() && parenCount > 0) {
                    if (content[front] == '(') parenCount++;
                    else if (content[front] == ')') parenCount--;
                    if (parenCount > 0) front++;
                }
                
                if (parenCount == 0) {
                    std::string argString = content.substr(argStart, front - argStart);
                    result.Arguments = ParseArguments(argString);
                    front++; // Skip ')'
                }
            } else {
                // Space-separated arguments or other patterns
                // This depends on the specific pattern requirements
                // For now, collect until next keyword or delimiter
                size_t argStart = front;
                while (front < content.length() && 
                       content[front] != ';' && 
                       content[front] != '\n' &&
                       content[front] != '{') {
                    front++;
                }
                
                std::string argString = content.substr(argStart, front - argStart);
                if (!argString.empty()) {
                    result.Arguments.push_back(argString);
                }
            }
            
            result.End = front;
            results.push_back(result);
            back = front;
        } else {
            // Not a keyword, move both pointers
            back = front;
        }
    }
    
    return results;
}

std::string CJMODProcessor::CapturePrefix(const std::string& content, size_t keywordPos, const std::string& keyword) {
    // Look backwards for prefix that should be captured
    // For example, for "arg ** arg2", capture "arg " before "**"
    
    if (keywordPos == 0) return "";
    
    size_t prefixEnd = keywordPos;
    size_t prefixStart = keywordPos;
    
    // Skip whitespace before keyword
    while (prefixStart > 0 && std::isspace(content[prefixStart - 1])) {
        prefixStart--;
    }
    
    // Check if there's an identifier before the keyword
    if (prefixStart > 0 && (std::isalnum(content[prefixStart - 1]) || content[prefixStart - 1] == '_')) {
        // Capture the identifier
        while (prefixStart > 0 && (std::isalnum(content[prefixStart - 1]) || content[prefixStart - 1] == '_')) {
            prefixStart--;
        }
        
        return content.substr(prefixStart, prefixEnd - prefixStart);
    }
    
    return "";
}

bool CJMODProcessor::IsKeywordBoundary(char before, char after) const {
    // Check if the characters before and after form a word boundary
    bool beforeBoundary = !std::isalnum(before) && before != '_';
    bool afterBoundary = !std::isalnum(after) && after != '_';
    return beforeBoundary && afterBoundary;
}

std::vector<std::string> CJMODProcessor::ParseArguments(const std::string& argString) {
    std::vector<std::string> args;
    std::stringstream ss(argString);
    std::string arg;
    
    // Simple comma-separated parsing
    // In a real implementation, this would handle nested structures
    while (std::getline(ss, arg, ',')) {
        // Trim whitespace
        arg.erase(0, arg.find_first_not_of(" \t\n\r"));
        arg.erase(arg.find_last_not_of(" \t\n\r") + 1);
        
        if (!arg.empty()) {
            args.push_back(arg);
        }
    }
    
    return args;
}

std::string CJMODProcessor::GenerateReplacement(const std::string& keyword, const std::vector<std::string>& args) {
    // This would use the registered handler
    auto patternInfo = CJMODPatternRegistry::GetInstance().GetPatternInfo(keyword);
    if (patternInfo) {
        return patternInfo->Handler(args);
    }
    return "";
}

}