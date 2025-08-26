add_library(CHTLCommon STATIC
    Token.cpp
    Error.cpp
    Location.cpp
    SourceFile.cpp
)

target_include_directories(CHTLCommon PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)