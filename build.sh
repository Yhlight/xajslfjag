#!/bin/bash

# CHTL项目构建脚本
# 使用方法: ./build.sh [选项]
# 选项:
#   clean    - 清理构建目录
#   debug    - Debug模式构建
#   release  - Release模式构建（默认）
#   test     - 构建并运行测试
#   install  - 安装到系统
#   package  - 创建安装包
#   all      - 完整构建流程

set -e  # 遇到错误立即退出

# 颜色定义
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# 打印函数
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

print_header() {
    echo -e "${BLUE}================================${NC}"
    echo -e "${BLUE}$1${NC}"
    echo -e "${BLUE}================================${NC}"
}

# 检查依赖
check_dependencies() {
    print_info "检查构建依赖..."
    
    # 检查CMake
    if ! command -v cmake &> /dev/null; then
        print_error "CMake未安装，请先安装CMake 3.15或更高版本"
        exit 1
    fi
    
    CMAKE_VERSION=$(cmake --version | head -n1 | awk '{print $3}')
    print_info "CMake版本: $CMAKE_VERSION"
    
    # 检查编译器
    if command -v g++ &> /dev/null; then
        GCC_VERSION=$(g++ --version | head -n1)
        print_info "编译器: $GCC_VERSION"
    elif command -v clang++ &> /dev/null; then
        CLANG_VERSION=$(clang++ --version | head -n1)
        print_info "编译器: $CLANG_VERSION"
    else
        print_error "未找到C++编译器，请安装g++或clang++"
        exit 1
    fi
    
    # 检查线程库支持
    print_info "检查线程库支持..."
    if command -v pkg-config &> /dev/null; then
        if pkg-config --exists threads; then
            print_info "线程库支持: 可用"
        fi
    fi
    
    print_success "依赖检查完成"
}

# 设置构建环境
setup_build_env() {
    print_info "设置构建环境..."
    
    # 获取脚本所在目录
    SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
    PROJECT_ROOT="$SCRIPT_DIR"
    BUILD_DIR="$PROJECT_ROOT/build"
    INSTALL_DIR="$PROJECT_ROOT/install"
    
    # 默认构建类型
    BUILD_TYPE="${BUILD_TYPE:-Release}"
    
    print_info "项目根目录: $PROJECT_ROOT"
    print_info "构建目录: $BUILD_DIR"
    print_info "安装目录: $INSTALL_DIR"
    print_info "构建类型: $BUILD_TYPE"
}

# 清理构建目录
clean_build() {
    print_header "清理构建目录"
    
    if [ -d "$BUILD_DIR" ]; then
        print_info "删除构建目录: $BUILD_DIR"
        rm -rf "$BUILD_DIR"
    fi
    
    if [ -d "$INSTALL_DIR" ]; then
        print_info "删除安装目录: $INSTALL_DIR"
        rm -rf "$INSTALL_DIR"
    fi
    
    # 清理生成的测试文件
    find "$PROJECT_ROOT" -name "test_output.*" -delete 2>/dev/null || true
    find "$PROJECT_ROOT" -name "*.log" -delete 2>/dev/null || true
    
    print_success "清理完成"
}

# 配置项目
configure_project() {
    print_header "配置CHTL项目"
    
    # 创建构建目录
    mkdir -p "$BUILD_DIR"
    cd "$BUILD_DIR"
    
    # CMake配置选项
    CMAKE_OPTIONS=(
        "-DCMAKE_BUILD_TYPE=$BUILD_TYPE"
        "-DCMAKE_INSTALL_PREFIX=$INSTALL_DIR"
        "-DCMAKE_EXPORT_COMPILE_COMMANDS=ON"
        "-DCHTL_ENABLE_PROFILING=OFF"
        "-DCHTL_ENABLE_SANITIZERS=OFF"
        "-DCHTL_BUILD_SHARED_LIBS=OFF"
    )
    
    # Debug模式特定选项
    if [ "$BUILD_TYPE" = "Debug" ]; then
        CMAKE_OPTIONS+=("-DCHTL_ENABLE_SANITIZERS=ON")
        print_info "Debug模式: 启用地址和未定义行为检测器"
    fi
    
    # 运行CMake配置
    print_info "运行CMake配置..."
    print_info "配置选项: ${CMAKE_OPTIONS[*]}"
    
    if cmake "${CMAKE_OPTIONS[@]}" "$PROJECT_ROOT"; then
        print_success "项目配置成功"
    else
        print_error "项目配置失败"
        exit 1
    fi
}

# 构建项目
build_project() {
    print_header "构建CHTL项目"
    
    cd "$BUILD_DIR"
    
    # 获取CPU核心数用于并行编译
    if command -v nproc &> /dev/null; then
        JOBS=$(nproc)
    elif command -v sysctl &> /dev/null; then
        JOBS=$(sysctl -n hw.ncpu)
    else
        JOBS=4
    fi
    
    print_info "使用 $JOBS 个并行任务进行编译"
    
    # 构建项目
    if cmake --build . --config "$BUILD_TYPE" -j "$JOBS"; then
        print_success "项目构建成功"
    else
        print_error "项目构建失败"
        exit 1
    fi
    
    # 显示构建结果
    print_info "构建产物:"
    if [ -f "chtl_compiler" ] || [ -f "chtl_compiler.exe" ]; then
        print_info "  ✓ CHTL编译器"
    fi
    
    if [ -f "libchtl_core.a" ] || [ -f "chtl_core.lib" ]; then
        print_info "  ✓ CHTL核心库"
    fi
    
    # 统计测试可执行文件
    TEST_COUNT=$(find . -name "test_*" -executable -type f | wc -l)
    if [ "$TEST_COUNT" -gt 0 ]; then
        print_info "  ✓ $TEST_COUNT 个测试程序"
    fi
}

# 运行测试
run_tests() {
    print_header "运行CHTL测试"
    
    cd "$BUILD_DIR"
    
    # 查找所有测试可执行文件
    TEST_FILES=($(find . -name "test_*" -executable -type f | sort))
    
    if [ ${#TEST_FILES[@]} -eq 0 ]; then
        print_warning "未找到测试文件"
        return
    fi
    
    print_info "找到 ${#TEST_FILES[@]} 个测试程序"
    
    # 运行每个测试
    PASSED_TESTS=0
    FAILED_TESTS=0
    
    for test_file in "${TEST_FILES[@]}"; do
        test_name=$(basename "$test_file")
        print_info "运行测试: $test_name"
        
        if timeout 60 "$test_file"; then
            print_success "  ✓ $test_name 通过"
            ((PASSED_TESTS++))
        else
            print_error "  ✗ $test_name 失败"
            ((FAILED_TESTS++))
        fi
        echo
    done
    
    # 测试结果汇总
    print_header "测试结果汇总"
    print_info "通过: $PASSED_TESTS"
    print_info "失败: $FAILED_TESTS"
    print_info "总计: $((PASSED_TESTS + FAILED_TESTS))"
    
    if [ "$FAILED_TESTS" -eq 0 ]; then
        print_success "所有测试通过！"
    else
        print_error "有 $FAILED_TESTS 个测试失败"
        exit 1
    fi
}

# 安装项目
install_project() {
    print_header "安装CHTL项目"
    
    cd "$BUILD_DIR"
    
    if cmake --install . --config "$BUILD_TYPE"; then
        print_success "项目安装成功"
        print_info "安装位置: $INSTALL_DIR"
        
        # 显示安装内容
        if [ -d "$INSTALL_DIR" ]; then
            print_info "安装内容:"
            find "$INSTALL_DIR" -type f | head -20 | while read -r file; do
                rel_path=${file#$INSTALL_DIR/}
                print_info "  $rel_path"
            done
            
            total_files=$(find "$INSTALL_DIR" -type f | wc -l)
            if [ "$total_files" -gt 20 ]; then
                print_info "  ... 以及其他 $((total_files - 20)) 个文件"
            fi
        fi
    else
        print_error "项目安装失败"
        exit 1
    fi
}

# 创建安装包
create_package() {
    print_header "创建CHTL安装包"
    
    cd "$BUILD_DIR"
    
    if command -v cpack &> /dev/null; then
        print_info "使用CPack创建安装包..."
        
        if cpack -C "$BUILD_TYPE"; then
            print_success "安装包创建成功"
            
            # 显示创建的包
            find . -name "*.tar.gz" -o -name "*.zip" -o -name "*.deb" -o -name "*.rpm" -o -name "*.dmg" | while read -r package; do
                package_name=$(basename "$package")
                package_size=$(du -h "$package" | cut -f1)
                print_info "  $package_name ($package_size)"
            done
        else
            print_error "安装包创建失败"
            exit 1
        fi
    else
        print_warning "CPack未安装，跳过包创建"
    fi
}

# 显示帮助信息
show_help() {
    echo "CHTL项目构建脚本"
    echo
    echo "使用方法: $0 [选项]"
    echo
    echo "选项:"
    echo "  clean     清理构建目录"
    echo "  debug     Debug模式构建"
    echo "  release   Release模式构建（默认）"
    echo "  test      构建并运行测试"
    echo "  install   安装到指定目录"
    echo "  package   创建安装包"
    echo "  all       完整构建流程（配置+构建+测试+安装）"
    echo "  help      显示此帮助信息"
    echo
    echo "环境变量:"
    echo "  BUILD_TYPE   构建类型 (Debug|Release|RelWithDebInfo|MinSizeRel)"
    echo "  CC           C编译器"
    echo "  CXX          C++编译器"
    echo
    echo "示例:"
    echo "  $0 clean"
    echo "  $0 debug"
    echo "  BUILD_TYPE=Debug $0 test"
    echo "  $0 all"
}

# 主函数
main() {
    print_header "CHTL项目构建系统"
    
    # 解析命令行参数
    ACTION="${1:-release}"
    
    case "$ACTION" in
        "clean")
            setup_build_env
            clean_build
            ;;
        "debug")
            BUILD_TYPE="Debug"
            setup_build_env
            check_dependencies
            configure_project
            build_project
            ;;
        "release")
            BUILD_TYPE="Release"
            setup_build_env
            check_dependencies
            configure_project
            build_project
            ;;
        "test")
            setup_build_env
            check_dependencies
            if [ ! -f "$BUILD_DIR/Makefile" ] && [ ! -f "$BUILD_DIR/build.ninja" ]; then
                configure_project
            fi
            build_project
            run_tests
            ;;
        "install")
            setup_build_env
            if [ ! -f "$BUILD_DIR/Makefile" ] && [ ! -f "$BUILD_DIR/build.ninja" ]; then
                check_dependencies
                configure_project
                build_project
            fi
            install_project
            ;;
        "package")
            setup_build_env
            if [ ! -f "$BUILD_DIR/Makefile" ] && [ ! -f "$BUILD_DIR/build.ninja" ]; then
                check_dependencies
                configure_project
                build_project
            fi
            create_package
            ;;
        "all")
            setup_build_env
            check_dependencies
            configure_project
            build_project
            run_tests
            install_project
            create_package
            ;;
        "help"|"-h"|"--help")
            show_help
            ;;
        *)
            print_error "未知选项: $ACTION"
            echo
            show_help
            exit 1
            ;;
    esac
    
    print_success "构建脚本执行完成"
}

# 运行主函数
main "$@"