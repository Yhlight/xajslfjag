# 模块系统构建配置

# 源文件
set(MODULE_SOURCES
    CMODSystem/CMODManager.cpp
    CMODSystem/CMODPackager.cpp
    CMODSystem/CMODLoader.cpp
    CJMODSystem/CJMODManager.cpp
    CJMODSystem/CJMODLoader.cpp
    CJMODSystem/CJMODInterface.cpp
    ModuleRegistry.cpp
    DependencyResolver.cpp
)

# 头文件
set(MODULE_HEADERS
    CMODSystem/CMODManager.h
    CMODSystem/CMODPackager.h
    CMODSystem/CMODLoader.h
    CJMODSystem/CJMODManager.h
    CJMODSystem/CJMODLoader.h
    CJMODSystem/CJMODInterface.h
    ModuleRegistry.h
    DependencyResolver.h
)

# 创建模块系统库
add_library(module_system STATIC ${MODULE_SOURCES} ${MODULE_HEADERS})

# 设置目标属性
set_target_properties(module_system PROPERTIES
    OUTPUT_NAME "module_system"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 包含目录
target_include_directories(module_system PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

# 安装规则
install(TARGETS module_system
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES ${MODULE_HEADERS}
    DESTINATION include/module_system
)