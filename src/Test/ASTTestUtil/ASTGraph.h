#pragma once
#include "../../CHTL/CHTLNode/BaseNode.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

namespace CHTL {
namespace Test {

/**
 * 图形输出格式枚举
 */
enum class GraphFormat {
    DOT,            // Graphviz DOT格式
    SVG,            // SVG矢量图
    PNG,            // PNG位图  
    PDF,            // PDF文档
    HTML,           // HTML网页
    ASCII,          // ASCII艺术图
    MERMAID,        // Mermaid图表
    PLANTUML        // PlantUML图表
};

/**
 * 图形布局类型
 */
enum class GraphLayout {
    TREE,           // 树状布局
    HIERARCHICAL,   // 分层布局
    CIRCULAR,       // 环形布局
    FORCE_DIRECTED, // 力导向布局
    GRID,           // 网格布局
    RADIAL          // 径向布局
};

/**
 * AST图形生成工具类
 * 提供多种格式的AST可视化图形
 */
class ASTGraph {
public:
    /**
     * 构造函数
     */
    ASTGraph(GraphFormat format = GraphFormat::DOT, GraphLayout layout = GraphLayout::TREE);
    
    /**
     * 设置图形格式
     */
    void setFormat(GraphFormat format);
    
    /**
     * 设置布局类型
     */
    void setLayout(GraphLayout layout);
    
    /**
     * 设置图形标题
     */
    void setTitle(const std::string& title);
    
    /**
     * 启用/禁用节点标签
     */
    void setShowNodeLabels(bool show);
    
    /**
     * 启用/禁用边标签
     */
    void setShowEdgeLabels(bool show);
    
    /**
     * 启用/禁用节点属性
     */
    void setShowNodeAttributes(bool show);
    
    /**
     * 设置颜色主题
     */
    void setColorTheme(const std::string& theme);
    
    /**
     * 生成AST图形
     */
    std::string generateGraph(const NodePtr& root);
    
    /**
     * 保存图形到文件
     */
    bool saveGraphToFile(const NodePtr& root, const std::string& filename);
    
    /**
     * 生成交互式HTML图形
     */
    std::string generateInteractiveHTML(const NodePtr& root);
    
    /**
     * 生成缩略图
     */
    std::string generateThumbnail(const NodePtr& root, int width = 200, int height = 150);

private:
    GraphFormat m_format;           // 图形格式
    GraphLayout m_layout;           // 布局类型
    std::string m_title;            // 图形标题
    bool m_showNodeLabels;          // 显示节点标签
    bool m_showEdgeLabels;          // 显示边标签
    bool m_showNodeAttributes;      // 显示节点属性
    std::string m_colorTheme;       // 颜色主题
    
    /**
     * 生成DOT格式图形
     */
    std::string generateDOTGraph(const NodePtr& root) const;
    
    /**
     * 生成SVG格式图形
     */
    std::string generateSVGGraph(const NodePtr& root) const;
    
    /**
     * 生成HTML格式图形
     */
    std::string generateHTMLGraph(const NodePtr& root) const;
    
    /**
     * 生成ASCII艺术图形
     */
    std::string generateASCIIGraph(const NodePtr& root) const;
    
    /**
     * 生成Mermaid格式图形
     */
    std::string generateMermaidGraph(const NodePtr& root) const;
    
    /**
     * 生成PlantUML格式图形
     */
    std::string generatePlantUMLGraph(const NodePtr& root) const;
    
    /**
     * 获取节点的DOT属性
     */
    std::string getNodeDOTAttributes(const NodePtr& node) const;
    
    /**
     * 获取边的DOT属性
     */
    std::string getEdgeDOTAttributes(const NodePtr& parent, const NodePtr& child) const;
    
    /**
     * 获取节点颜色
     */
    std::string getNodeColor(CHTLNodeType type) const;
    
    /**
     * 获取节点形状
     */
    std::string getNodeShape(CHTLNodeType type) const;
    
    /**
     * 获取节点标签
     */
    std::string getNodeLabel(const NodePtr& node) const;
    
    /**
     * 生成唯一节点ID
     */
    std::string generateNodeId(const NodePtr& node) const;
    
    /**
     * 递归生成DOT节点
     */
    void generateDOTNodes(const NodePtr& node, std::string& result, std::unordered_map<const BaseNode*, std::string>& nodeIds) const;
    
    /**
     * 递归生成DOT边
     */
    void generateDOTEdges(const NodePtr& node, std::string& result, const std::unordered_map<const BaseNode*, std::string>& nodeIds) const;
    
    /**
     * 计算节点位置（用于布局）
     */
    std::unordered_map<const BaseNode*, std::pair<double, double>> calculateNodePositions(const NodePtr& root) const;
};

/**
 * AST流程图生成器
 */
class ASTFlowChart {
public:
    /**
     * 生成控制流图
     */
    static std::string generateControlFlowChart(const NodePtr& root);
    
    /**
     * 生成数据流图
     */
    static std::string generateDataFlowChart(const NodePtr& root);
    
    /**
     * 生成依赖关系图
     */
    static std::string generateDependencyChart(const NodePtr& root);
    
    /**
     * 生成继承关系图
     */
    static std::string generateInheritanceChart(const NodePtr& root);
    
    /**
     * 生成时序图
     */
    static std::string generateSequenceChart(const NodePtr& root);

private:
    /**
     * 分析控制流
     */
    static void analyzeControlFlow(const NodePtr& node, std::vector<std::pair<std::string, std::string>>& flows);
    
    /**
     * 分析数据流
     */
    static void analyzeDataFlow(const NodePtr& node, std::vector<std::pair<std::string, std::string>>& flows);
    
    /**
     * 分析依赖关系
     */
    static void analyzeDependencies(const NodePtr& node, std::vector<std::pair<std::string, std::string>>& dependencies);
};

/**
 * AST热力图生成器
 */
class ASTHeatMap {
public:
    /**
     * 生成节点类型热力图
     */
    static std::string generateTypeHeatMap(const NodePtr& root, int width = 80, int height = 40);
    
    /**
     * 生成深度热力图
     */
    static std::string generateDepthHeatMap(const NodePtr& root, int width = 80, int height = 40);
    
    /**
     * 生成复杂度热力图
     */
    static std::string generateComplexityHeatMap(const NodePtr& root, int width = 80, int height = 40);
    
    /**
     * 生成访问频率热力图
     */
    static std::string generateAccessHeatMap(const NodePtr& root, const std::unordered_map<const BaseNode*, size_t>& accessCounts, int width = 80, int height = 40);

private:
    /**
     * 计算节点复杂度
     */
    static double calculateNodeComplexity(const NodePtr& node);
    
    /**
     * 映射值到颜色
     */
    static char mapValueToColor(double value, double minValue, double maxValue);
    
    /**
     * 生成热力图网格
     */
    static std::vector<std::vector<double>> generateHeatGrid(const NodePtr& root, int width, int height, 
                                                           std::function<double(const NodePtr&)> valueExtractor);
};

/**
 * AST 3D可视化生成器
 */
class AST3DVisualizer {
public:
    /**
     * 生成3D树状图（Three.js格式）
     */
    static std::string generate3DTree(const NodePtr& root);
    
    /**
     * 生成3D网格图
     */
    static std::string generate3DNetwork(const NodePtr& root);
    
    /**
     * 生成VR兼容的3D场景
     */
    static std::string generateVRScene(const NodePtr& root);
    
    /**
     * 生成WebGL场景
     */
    static std::string generateWebGLScene(const NodePtr& root);

private:
    /**
     * 计算3D坐标
     */
    static std::unordered_map<const BaseNode*, std::tuple<double, double, double>> calculate3DPositions(const NodePtr& root);
    
    /**
     * 生成3D模型数据
     */
    static std::string generate3DModelData(const NodePtr& root);
};

} // namespace Test
} // namespace CHTL