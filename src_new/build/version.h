#pragma once

#define CHTL_VERSION_MAJOR 1
#define CHTL_VERSION_MINOR 0
#define CHTL_VERSION_PATCH 0
#define CHTL_VERSION_STRING "1.0.0"

#define CHTL_BUILD_TYPE "Debug"
#define CHTL_COMPILER "Clang"
#define CHTL_BUILD_DATE __DATE__
#define CHTL_BUILD_TIME __TIME__

namespace CHTL {
namespace Version {

constexpr int MAJOR = CHTL_VERSION_MAJOR;
constexpr int MINOR = CHTL_VERSION_MINOR;
constexpr int PATCH = CHTL_VERSION_PATCH;
constexpr const char* STRING = CHTL_VERSION_STRING;
constexpr const char* BUILD_TYPE = CHTL_BUILD_TYPE;
constexpr const char* COMPILER = CHTL_COMPILER;
constexpr const char* BUILD_DATE = CHTL_BUILD_DATE;
constexpr const char* BUILD_TIME = CHTL_BUILD_TIME;

} // namespace Version
} // namespace CHTL
