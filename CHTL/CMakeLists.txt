cmake_minimum_required(VERSION 3.10)
project(CHTL)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 设置UTF-8支持
add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)

# 添加子目录
add_subdirectory(CHTLLexer)
add_subdirectory(CHTLContext)
# add_subdirectory(CHTLState)
# add_subdirectory(CHTLNode)
# add_subdirectory(CHTLParser)
# add_subdirectory(CHTLGenerator)
# add_subdirectory(CHTLLoader)
# add_subdirectory(CHTLManage)
# add_subdirectory(CMODSystem)