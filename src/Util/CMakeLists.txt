# 工具系统库

file(GLOB_RECURSE UTIL_SOURCES
    "FileSystem/*.cpp"
    "ZIPUtil/*.cpp"
)

file(GLOB_RECURSE UTIL_HEADERS
    "FileSystem/*.h"
    "ZIPUtil/*.h"
)

add_library(Util_System STATIC
    ${UTIL_SOURCES}
    ${UTIL_HEADERS}
)

target_include_directories(Util_System PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_features(Util_System PUBLIC cxx_std_17)

message(STATUS "工具系统库配置完成")