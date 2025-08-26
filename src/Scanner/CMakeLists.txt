add_library(CHTLScanner STATIC
    UnifiedScanner.cpp
    CodeSlice.cpp
    ScannerState.cpp
)

target_link_libraries(CHTLScanner PUBLIC
    CHTLCommon
)

target_include_directories(CHTLScanner PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)