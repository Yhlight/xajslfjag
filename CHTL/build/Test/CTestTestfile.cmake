# CMake generated Testfile for 
# Source directory: /workspace/CHTL/Test
# Build directory: /workspace/CHTL/build/Test
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(LexerTest "/workspace/CHTL/build/bin/test_lexer")
set_tests_properties(LexerTest PROPERTIES  _BACKTRACE_TRIPLES "/workspace/CHTL/Test/CMakeLists.txt;51;add_test;/workspace/CHTL/Test/CMakeLists.txt;0;")
add_test(CHTLJSLexerTest "/workspace/CHTL/build/bin/test_chtljs_lexer")
set_tests_properties(CHTLJSLexerTest PROPERTIES  _BACKTRACE_TRIPLES "/workspace/CHTL/Test/CMakeLists.txt;52;add_test;/workspace/CHTL/Test/CMakeLists.txt;0;")
add_test(UnquotedLiteralsTest "/workspace/CHTL/build/bin/test_unquoted_literals")
set_tests_properties(UnquotedLiteralsTest PROPERTIES  _BACKTRACE_TRIPLES "/workspace/CHTL/Test/CMakeLists.txt;53;add_test;/workspace/CHTL/Test/CMakeLists.txt;0;")
add_test(CHTLFeaturesTest "/workspace/CHTL/build/bin/test_chtl_features")
set_tests_properties(CHTLFeaturesTest PROPERTIES  _BACKTRACE_TRIPLES "/workspace/CHTL/Test/CMakeLists.txt;54;add_test;/workspace/CHTL/Test/CMakeLists.txt;0;")
