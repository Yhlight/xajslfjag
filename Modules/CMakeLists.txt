# Modules CMakeLists.txt
# CHTL模块系统

# 创建占位文件（后续实现）
file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/placeholder.cpp "// Placeholder for Modules\n")

# 创建Modules库（暂时为空）
add_library(CHTLModulesLib STATIC ${CMAKE_CURRENT_BINARY_DIR}/placeholder.cpp)