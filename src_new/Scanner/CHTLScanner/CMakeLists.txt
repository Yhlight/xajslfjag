# CHTLScanner library
project(CHTLScanner)

# Add source files
set(SCANNER_SOURCES
    UnifiedScanner.cpp
)

set(SCANNER_HEADERS
    UnifiedScanner.h
)

# Create the static library
add_library(CHTLScanner STATIC ${SCANNER_SOURCES} ${SCANNER_HEADERS})

# Set include directories
target_include_directories(CHTLScanner PUBLIC 
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../Error
)

# Link with required libraries
target_link_libraries(CHTLScanner 
    CHTLError
)

# Set C++ standard
set_target_properties(CHTLScanner PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)