# ThirdParty模块

# 设置ThirdParty源文件
set(THIRDPARTY_SOURCES
    # 这里将添加第三方库的源文件
)

# 创建ThirdParty静态库
add_library(ThirdParty STATIC ${THIRDPARTY_SOURCES})

# 设置目标属性
set_target_properties(ThirdParty PROPERTIES
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)