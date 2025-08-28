# 编译器调度器构建配置

# 源文件
set(DISPATCHER_SOURCES
    CompilerDispatcher.cpp
    TaskScheduler.cpp
    ResultCollector.cpp
    CompilerManager.cpp
)

# 头文件
set(DISPATCHER_HEADERS
    CompilerDispatcher.h
    TaskScheduler.h
    ResultCollector.h
    CompilerManager.h
)

# 创建调度器库
add_library(dispatcher STATIC ${DISPATCHER_SOURCES} ${DISPATCHER_HEADERS})

# 设置目标属性
set_target_properties(dispatcher PROPERTIES
    OUTPUT_NAME "dispatcher"
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

# 包含目录
target_include_directories(dispatcher PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/src
)

# 安装规则
install(TARGETS dispatcher
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
)

install(FILES ${DISPATCHER_HEADERS}
    DESTINATION include/dispatcher
)