# CHTLCompiler library
project(CHTLCompiler)

# Add source files
set(COMPILER_SOURCES
    CompilerDispatcher.cpp
    CHTLJSCompiler.cpp
    ANTLRCompiler.cpp
)

set(COMPILER_HEADERS
    CompilerDispatcher.h
    CHTLJSCompiler.h
    ANTLRCompiler.h
)

# Create the static library
add_library(CHTLCompiler STATIC ${COMPILER_SOURCES} ${COMPILER_HEADERS})

# Set include directories
target_include_directories(CHTLCompiler PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../Error
    ${CMAKE_CURRENT_SOURCE_DIR}/../CHTLScanner
    ${CMAKE_CURRENT_SOURCE_DIR}/../CHTLParser
    ${CMAKE_CURRENT_SOURCE_DIR}/../CHTLGenerator
)

# Link with required libraries
target_link_libraries(CHTLCompiler 
    CHTLError
    CHTLScanner
    CHTLParser
    CHTLGenerator
)

# Set C++ standard
set_target_properties(CHTLCompiler PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)