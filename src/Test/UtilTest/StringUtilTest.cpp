#include "../CHTLTestSuite.h"
#include "../../Util/StringUtil.h"

using namespace CHTL::Test;

CHTL_TEST_SUITE(StringUtil) {
    CHTL_ADD_TEST(StringUtil, Trim);
    CHTL_ADD_TEST(StringUtil, Split);
    CHTL_ADD_TEST(StringUtil, Join);
    CHTL_ADD_TEST(StringUtil, Replace);
    CHTL_ADD_TEST(StringUtil, StartsWith);
    CHTL_ADD_TEST(StringUtil, EndsWith);
    CHTL_ADD_TEST(StringUtil, ToUpperLower);
}

CHTL_TEST(StringUtil, Trim) {
    assertEqual(StringUtil::trim("  hello  "), "hello");
    assertEqual(StringUtil::trim("\t\nworld\r\n"), "world");
    assertEqual(StringUtil::trim("   "), "");
    assertEqual(StringUtil::trim("no_trim"), "no_trim");
    assertEqual(StringUtil::ltrim("  left"), "left");
    assertEqual(StringUtil::rtrim("right  "), "right");
}

CHTL_TEST(StringUtil, Split) {
    auto parts = StringUtil::split("a,b,c", ",");
    assertEqual(parts.size(), 3);
    assertEqual(parts[0], "a");
    assertEqual(parts[1], "b");
    assertEqual(parts[2], "c");
    
    parts = StringUtil::split("hello::world", "::");
    assertEqual(parts.size(), 2);
    assertEqual(parts[0], "hello");
    assertEqual(parts[1], "world");
    
    parts = StringUtil::split("single", ",");
    assertEqual(parts.size(), 1);
    assertEqual(parts[0], "single");
}

CHTL_TEST(StringUtil, Join) {
    std::vector<std::string> parts = {"a", "b", "c"};
    assertEqual(StringUtil::join(parts, ","), "a,b,c");
    assertEqual(StringUtil::join(parts, " - "), "a - b - c");
    
    std::vector<std::string> single = {"one"};
    assertEqual(StringUtil::join(single, ","), "one");
    
    std::vector<std::string> empty;
    assertEqual(StringUtil::join(empty, ","), "");
}

CHTL_TEST(StringUtil, Replace) {
    assertEqual(StringUtil::replace("hello world", "world", "universe"), "hello universe");
    assertEqual(StringUtil::replace("aaa", "a", "b"), "bbb");
    assertEqual(StringUtil::replace("test", "x", "y"), "test");
    assertEqual(StringUtil::replaceAll("ababa", "ab", "x"), "xxa");
}

CHTL_TEST(StringUtil, StartsWith) {
    assertTrue(StringUtil::startsWith("hello world", "hello"));
    assertTrue(StringUtil::startsWith("test", ""));
    assertFalse(StringUtil::startsWith("hello", "world"));
    assertFalse(StringUtil::startsWith("hi", "hello"));
}

CHTL_TEST(StringUtil, EndsWith) {
    assertTrue(StringUtil::endsWith("hello world", "world"));
    assertTrue(StringUtil::endsWith("test", ""));
    assertFalse(StringUtil::endsWith("hello", "world"));
    assertFalse(StringUtil::endsWith("test", "testing"));
}

CHTL_TEST(StringUtil, ToUpperLower) {
    assertEqual(StringUtil::toUpper("hello"), "HELLO");
    assertEqual(StringUtil::toLower("WORLD"), "world");
    assertEqual(StringUtil::toUpper("Test123"), "TEST123");
    assertEqual(StringUtil::toLower("Test123"), "test123");
}