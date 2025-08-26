#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>

namespace Chtholly {
namespace CJMOD {

/**
 * PrintMyLove - 珂朵莉的图片转字符像素块功能
 * 严格按照语法文档实现：可以将一张图片变成字符像素块的形式，支持ASCII或Pixel模式
 */
class PrintMyLove {
public:
    /**
     * 转换模式
     */
    enum class ConversionMode {
        ASCII,          // ASCII字符模式
        PIXEL           // 像素块模式
    };
    
    /**
     * 缩放策略
     */
    enum class ScaleStrategy {
        PROPORTIONAL,   // 等比缩放（默认）
        STRETCH,        // 拉伸缩放
        CROP            // 裁剪缩放
    };
    
    /**
     * 转换参数结构
     */
    struct ConversionParams {
        std::string url;                    // 图片URL或路径
        ConversionMode mode;                // 转换模式
        int width;                          // 目标宽度
        int height;                         // 目标高度
        double scale;                       // 缩放倍数
        ScaleStrategy strategy;             // 缩放策略
        bool use_color;                     // 是否使用颜色
        int brightness_threshold;           // 亮度阈值
        std::string custom_charset;         // 自定义字符集
        
        ConversionParams() 
            : mode(ConversionMode::ASCII)
            , width(80)
            , height(40)
            , scale(1.0)
            , strategy(ScaleStrategy::PROPORTIONAL)
            , use_color(false)
            , brightness_threshold(128)
            , custom_charset(" .:-=+*#%@") {}
    };
    
    /**
     * 转换结果
     */
    struct ConversionResult {
        std::string ascii_art;              // ASCII艺术字符串
        std::vector<std::string> lines;     // 按行分割的结果
        int final_width;                    // 最终宽度
        int final_height;                   // 最终高度
        bool success;                       // 转换是否成功
        std::string error_message;          // 错误信息
        
        ConversionResult() : final_width(0), final_height(0), success(false) {}
    };

public:
    /**
     * 转换图片为字符艺术
     * @param params 转换参数
     * @return 转换结果
     */
    static ConversionResult convert(const ConversionParams& params);
    
    /**
     * 从URL加载图片并转换
     * @param url 图片URL
     * @param mode 转换模式
     * @param width 宽度
     * @param height 高度
     * @param scale 缩放倍数
     * @return 转换结果字符串
     */
    static std::string convertFromUrl(const std::string& url, 
                                    ConversionMode mode = ConversionMode::ASCII,
                                    int width = 80, 
                                    int height = 40, 
                                    double scale = 1.0);
    
    /**
     * 从文件加载图片并转换
     * @param file_path 文件路径
     * @param mode 转换模式
     * @param width 宽度
     * @param height 高度
     * @param scale 缩放倍数
     * @return 转换结果字符串
     */
    static std::string convertFromFile(const std::string& file_path,
                                     ConversionMode mode = ConversionMode::ASCII,
                                     int width = 80,
                                     int height = 40,
                                     double scale = 1.0);
    
    /**
     * 输出到控制台
     * @param result 转换结果
     * @param use_color 是否使用颜色输出
     */
    static void printToConsole(const ConversionResult& result, bool use_color = false);
    
    /**
     * 保存到文件
     * @param result 转换结果
     * @param file_path 保存路径
     * @return 是否保存成功
     */
    static bool saveToFile(const ConversionResult& result, const std::string& file_path);
    
    /**
     * 获取默认ASCII字符集
     * @return 字符集字符串
     */
    static std::string getDefaultCharset();
    
    /**
     * 获取高密度ASCII字符集
     * @return 字符集字符串
     */
    static std::string getHighDensityCharset();
    
    /**
     * 设置自定义字符集
     * @param charset 字符集
     */
    static void setCustomCharset(const std::string& charset);

private:
    // 图像处理相关
    struct ImageData {
        std::vector<std::vector<int>> pixels;  // 灰度像素数据
        int width;
        int height;
        
        ImageData() : width(0), height(0) {}
    };
    
    /**
     * 加载图片数据
     * @param source 图片源（URL或文件路径）
     * @param is_url 是否为URL
     * @return 图片数据
     */
    static ImageData loadImage(const std::string& source, bool is_url);
    
    /**
     * 缩放图片
     * @param image 原图数据
     * @param target_width 目标宽度
     * @param target_height 目标高度
     * @param strategy 缩放策略
     * @return 缩放后的图片数据
     */
    static ImageData scaleImage(const ImageData& image, 
                               int target_width, 
                               int target_height,
                               ScaleStrategy strategy);
    
    /**
     * 转换为ASCII
     * @param image 图片数据
     * @param charset 字符集
     * @param brightness_threshold 亮度阈值
     * @return ASCII字符串
     */
    static std::string convertToASCII(const ImageData& image, 
                                    const std::string& charset,
                                    int brightness_threshold);
    
    /**
     * 转换为像素块
     * @param image 图片数据
     * @param use_color 是否使用颜色
     * @return 像素块字符串
     */
    static std::string convertToPixel(const ImageData& image, bool use_color);
    
    /**
     * 双线性插值
     * @param image 原图数据
     * @param x 目标x坐标（浮点）
     * @param y 目标y坐标（浮点）
     * @return 插值后的像素值
     */
    static int bilinearInterpolation(const ImageData& image, double x, double y);
    
    /**
     * 获取字符对应的亮度值
     * @param pixel_value 像素值
     * @param charset 字符集
     * @return 对应字符
     */
    static char getCharForPixel(int pixel_value, const std::string& charset);
    
    /**
     * ANSI颜色代码生成
     * @param r 红色分量
     * @param g 绿色分量
     * @param b 蓝色分量
     * @return ANSI颜色代码
     */
    static std::string getAnsiColor(int r, int g, int b);
    
    // 静态成员
    static std::string custom_charset_;
    static bool use_custom_charset_;
};

/**
 * PrintMyLoveJS - JavaScript接口封装
 * 用于CJMOD API的JavaScript绑定
 */
class PrintMyLoveJS {
public:
    /**
     * 创建CHTL JS函数绑定
     * @return JavaScript函数代码
     */
    static std::string createCHTLJSFunction();
    
    /**
     * 处理CHTL JS调用
     * @param params 参数字符串
     * @return 处理结果
     */
    static std::string handleCHTLJSCall(const std::string& params);
    
    /**
     * 解析CHTL JS参数
     * @param js_params JavaScript参数字符串
     * @return 解析后的参数结构
     */
    static PrintMyLove::ConversionParams parseCHTLJSParams(const std::string& js_params);
    
    /**
     * 生成JavaScript返回值
     * @param result 转换结果
     * @return JavaScript对象字符串
     */
    static std::string generateJSResult(const PrintMyLove::ConversionResult& result);

private:
    /**
     * 解析尺寸字符串（支持CSS单位、百分比、小数、纯数字）
     * @param size_str 尺寸字符串
     * @param reference_size 参考尺寸（用于百分比计算）
     * @return 解析后的像素值
     */
    static int parseSize(const std::string& size_str, int reference_size = 100);
    
    /**
     * 解析模式字符串
     * @param mode_str 模式字符串（"ASCII" 或 "Pixel"）
     * @return 转换模式枚举
     */
    static PrintMyLove::ConversionMode parseMode(const std::string& mode_str);
};

} // namespace CJMOD
} // namespace Chtholly