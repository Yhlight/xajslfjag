# CHTL项目源代码构建配置

# 设置编译选项
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17")

# 子目录
add_subdirectory(CHTL)
add_subdirectory(CHTL\ JS)
add_subdirectory(CSS)
add_subdirectory(JS)
add_subdirectory(Scanner)
add_subdirectory(CompilerDispatcher)
add_subdirectory(Util)
add_subdirectory(Module)

# 测试目录（可选）
option(BUILD_TESTS "Build test suite" OFF)
if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(Test)
endif()

# 第三方库目录（可选）
option(BUILD_THIRD_PARTY "Build third party libraries" OFF)
if(BUILD_THIRD_PARTY)
    add_subdirectory(ThirdParty)
endif()