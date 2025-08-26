# 添加子目录
add_subdirectory(Common)
add_subdirectory(Utils)
add_subdirectory(Scanner)
add_subdirectory(CHTL)
add_subdirectory(CHTLJS)
add_subdirectory(CSS)
add_subdirectory(JS)
add_subdirectory(Dispatcher)

# 包含目录
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Common
    ${CMAKE_CURRENT_SOURCE_DIR}/Utils
    ${CMAKE_CURRENT_SOURCE_DIR}/Scanner
    ${CMAKE_CURRENT_SOURCE_DIR}/CHTL
    ${CMAKE_CURRENT_SOURCE_DIR}/CHTLJS
    ${CMAKE_CURRENT_SOURCE_DIR}/CSS
    ${CMAKE_CURRENT_SOURCE_DIR}/JS
    ${CMAKE_CURRENT_SOURCE_DIR}/Dispatcher
)