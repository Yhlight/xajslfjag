#!/bin/bash

# CHTL编译器构建脚本
# 使用方法: ./build.sh [选项]

set -e  # 遇到错误立即退出

# 默认配置
BUILD_TYPE="Release"
BUILD_DIR="build"
INSTALL_PREFIX="/usr/local"
ENABLE_TESTS=false
ENABLE_DOCS=false
ENABLE_PACKAGE=false
ENABLE_STATIC_ANALYSIS=false
ENABLE_COVERAGE=false
ENABLE_PROFILING=false
JOBS=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
VERBOSE=false
CLEAN=false

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 打印带颜色的消息
print_message() {
    local color=$1
    local message=$2
    echo -e "${color}${message}${NC}"
}

print_success() {
    print_message "$GREEN" "✓ $1"
}

print_error() {
    print_message "$RED" "✗ $1"
}

print_warning() {
    print_message "$YELLOW" "⚠ $1"
}

print_info() {
    print_message "$BLUE" "ℹ $1"
}

# 显示帮助信息
show_help() {
    cat << EOF
CHTL编译器构建脚本

用法: $0 [选项]

选项:
    -h, --help              显示此帮助信息
    -t, --type TYPE         构建类型 (Debug|Release|MinSizeRel|RelWithDebInfo) [默认: Release]
    -d, --build-dir DIR     构建目录 [默认: build]
    -p, --prefix PATH       安装前缀 [默认: /usr/local]
    -j, --jobs N            并行作业数 [默认: CPU核心数]
    --tests                 启用测试构建
    --docs                  启用文档生成
    --package               启用包构建
    --static-analysis       启用静态分析
    --coverage              启用代码覆盖率 (仅Debug模式)
    --profiling             启用性能分析
    --clean                 清理构建目录
    --verbose               详细输出
    --install               构建后安装

构建类型说明:
    Debug           - 调试版本，包含调试信息，无优化
    Release         - 发布版本，完全优化，无调试信息
    MinSizeRel      - 最小体积发布版本
    RelWithDebInfo  - 带调试信息的发布版本

示例:
    $0                                    # 默认Release构建
    $0 --type Debug --tests --docs        # Debug构建，包含测试和文档
    $0 --package --install                # 构建并安装包
    $0 --clean                            # 清理构建目录
    $0 --static-analysis --coverage       # 启用静态分析和覆盖率

EOF
}

# 检查依赖
check_dependencies() {
    print_info "检查构建依赖..."
    
    # 检查CMake
    if ! command -v cmake &> /dev/null; then
        print_error "CMake未找到，请安装CMake 3.15+"
        exit 1
    fi
    
    local cmake_version=$(cmake --version | head -n1 | cut -d' ' -f3)
    print_success "找到CMake $cmake_version"
    
    # 检查编译器
    if command -v g++ &> /dev/null; then
        local gcc_version=$(g++ --version | head -n1)
        print_success "找到编译器: $gcc_version"
    elif command -v clang++ &> /dev/null; then
        local clang_version=$(clang++ --version | head -n1)
        print_success "找到编译器: $clang_version"
    else
        print_error "未找到C++编译器"
        exit 1
    fi
    
    # 检查ANTLR4（可选）
    if pkg-config --exists antlr4-runtime 2>/dev/null; then
        local antlr_version=$(pkg-config --modversion antlr4-runtime)
        print_success "找到ANTLR4: $antlr_version"
    else
        print_warning "未找到ANTLR4，将使用内置副本"
    fi
    
    # 检查可选工具
    if $ENABLE_STATIC_ANALYSIS; then
        if command -v clang-tidy &> /dev/null; then
            print_success "找到clang-tidy"
        else
            print_warning "未找到clang-tidy，静态分析可能不完整"
        fi
        
        if command -v cppcheck &> /dev/null; then
            print_success "找到cppcheck"
        else
            print_warning "未找到cppcheck"
        fi
    fi
    
    if $ENABLE_DOCS; then
        if command -v doxygen &> /dev/null; then
            print_success "找到Doxygen"
        else
            print_warning "未找到Doxygen，无法生成文档"
        fi
    fi
}

# 清理构建目录
clean_build() {
    if [ -d "$BUILD_DIR" ]; then
        print_info "清理构建目录: $BUILD_DIR"
        rm -rf "$BUILD_DIR"
        print_success "构建目录已清理"
    else
        print_info "构建目录不存在，无需清理"
    fi
}

# 配置构建
configure_build() {
    print_info "配置构建 (类型: $BUILD_TYPE, 目录: $BUILD_DIR)"
    
    # 创建构建目录
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    # 构建CMake参数
    local cmake_args=(
        "-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
        "-DCMAKE_INSTALL_PREFIX=$INSTALL_PREFIX"
        "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
    )
    
    if $ENABLE_TESTS; then
        cmake_args+=("-DBUILD_TESTS=ON")
    fi
    
    if $ENABLE_DOCS; then
        cmake_args+=("-DBUILD_DOCS=ON")
    fi
    
    if $ENABLE_PACKAGE; then
        cmake_args+=("-DBUILD_PACKAGE=ON")
    fi
    
    if $ENABLE_STATIC_ANALYSIS; then
        cmake_args+=("-DENABLE_STATIC_ANALYSIS=ON")
    fi
    
    if $ENABLE_COVERAGE && [ "$BUILD_TYPE" = "Debug" ]; then
        cmake_args+=("-DENABLE_COVERAGE=ON")
    fi
    
    if $ENABLE_PROFILING; then
        cmake_args+=("-DENABLE_PROFILING=ON")
    fi
    
    # 执行CMake配置
    if $VERBOSE; then
        cmake "${cmake_args[@]}" ..
    else
        cmake "${cmake_args[@]}" .. > /dev/null
    fi
    
    print_success "配置完成"
    cd ..
}

# 执行构建
build_project() {
    print_info "开始构建 (使用 $JOBS 个并行作业)"
    
    cd "$BUILD_DIR"
    
    local build_args=(
        "--build" "."
        "--config" "$BUILD_TYPE"
        "--parallel" "$JOBS"
    )
    
    if $VERBOSE; then
        build_args+=("--verbose")
    fi
    
    # 记录构建开始时间
    local start_time=$(date +%s)
    
    # 执行构建
    if cmake "${build_args[@]}"; then
        local end_time=$(date +%s)
        local duration=$((end_time - start_time))
        print_success "构建完成 (耗时: ${duration}秒)"
    else
        print_error "构建失败"
        cd ..
        exit 1
    fi
    
    cd ..
}

# 运行测试
run_tests() {
    if $ENABLE_TESTS; then
        print_info "运行测试..."
        cd "$BUILD_DIR"
        
        if ctest --output-on-failure; then
            print_success "所有测试通过"
        else
            print_error "部分测试失败"
            cd ..
            exit 1
        fi
        
        cd ..
    fi
}

# 生成文档
generate_docs() {
    if $ENABLE_DOCS; then
        print_info "生成文档..."
        cd "$BUILD_DIR"
        
        if cmake --build . --target docs; then
            print_success "文档生成完成"
            print_info "文档位置: $BUILD_DIR/docs/html/index.html"
        else
            print_warning "文档生成失败"
        fi
        
        cd ..
    fi
}

# 构建包
build_package() {
    if $ENABLE_PACKAGE; then
        print_info "构建安装包..."
        cd "$BUILD_DIR"
        
        if cpack; then
            print_success "安装包构建完成"
            print_info "安装包位置: $BUILD_DIR/"
        else
            print_error "安装包构建失败"
            cd ..
            exit 1
        fi
        
        cd ..
    fi
}

# 安装
install_project() {
    if $INSTALL; then
        print_info "安装到: $INSTALL_PREFIX"
        cd "$BUILD_DIR"
        
        if cmake --install .; then
            print_success "安装完成"
        else
            print_error "安装失败"
            cd ..
            exit 1
        fi
        
        cd ..
    fi
}

# 显示构建信息
show_build_info() {
    print_info "构建信息:"
    echo "  构建类型: $BUILD_TYPE"
    echo "  构建目录: $BUILD_DIR"
    echo "  安装前缀: $INSTALL_PREFIX"
    echo "  并行作业: $JOBS"
    echo "  启用测试: $ENABLE_TESTS"
    echo "  生成文档: $ENABLE_DOCS"
    echo "  构建包: $ENABLE_PACKAGE"
    echo "  静态分析: $ENABLE_STATIC_ANALYSIS"
    echo "  代码覆盖率: $ENABLE_COVERAGE"
    echo "  性能分析: $ENABLE_PROFILING"
}

# 主函数
main() {
    # 解析命令行参数
    while [[ $# -gt 0 ]]; do
        case $1 in
            -h|--help)
                show_help
                exit 0
                ;;
            -t|--type)
                BUILD_TYPE="$2"
                shift 2
                ;;
            -d|--build-dir)
                BUILD_DIR="$2"
                shift 2
                ;;
            -p|--prefix)
                INSTALL_PREFIX="$2"
                shift 2
                ;;
            -j|--jobs)
                JOBS="$2"
                shift 2
                ;;
            --tests)
                ENABLE_TESTS=true
                shift
                ;;
            --docs)
                ENABLE_DOCS=true
                shift
                ;;
            --package)
                ENABLE_PACKAGE=true
                shift
                ;;
            --static-analysis)
                ENABLE_STATIC_ANALYSIS=true
                shift
                ;;
            --coverage)
                ENABLE_COVERAGE=true
                shift
                ;;
            --profiling)
                ENABLE_PROFILING=true
                shift
                ;;
            --clean)
                CLEAN=true
                shift
                ;;
            --verbose)
                VERBOSE=true
                shift
                ;;
            --install)
                INSTALL=true
                shift
                ;;
            *)
                print_error "未知选项: $1"
                show_help
                exit 1
                ;;
        esac
    done
    
    # 验证构建类型
    case $BUILD_TYPE in
        Debug|Release|MinSizeRel|RelWithDebInfo)
            ;;
        *)
            print_error "无效的构建类型: $BUILD_TYPE"
            exit 1
            ;;
    esac
    
    print_info "CHTL编译器构建脚本"
    print_info "===================="
    
    # 如果只是清理，则执行清理并退出
    if $CLEAN; then
        clean_build
        exit 0
    fi
    
    # 显示构建信息
    show_build_info
    echo
    
    # 执行构建流程
    check_dependencies
    configure_build
    build_project
    run_tests
    generate_docs
    build_package
    install_project
    
    print_success "构建流程完成!"
    print_info "可执行文件位置: $BUILD_DIR/bin/chtl"
    
    if $ENABLE_TESTS; then
        print_info "测试报告: $BUILD_DIR/Testing/"
    fi
    
    if $ENABLE_DOCS; then
        print_info "文档: $BUILD_DIR/docs/html/"
    fi
    
    if $ENABLE_PACKAGE; then
        print_info "安装包: $BUILD_DIR/*.tar.gz (或其他格式)"
    fi
}

# 执行主函数
main "$@"