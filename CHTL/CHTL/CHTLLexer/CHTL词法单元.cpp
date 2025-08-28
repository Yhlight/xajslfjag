#include "CHTL词法单元.h"
#include <unordered_map>
#include <unordered_set>

namespace CHTL {
    namespace 核心 {
        
        CHTL词法单元::CHTL词法单元(CHTL词法单元类型 类型, const std::string& 值, 
                                  const CHTL词法单元位置& 位置)
            : 类型_(类型), 值_(值), 位置_(位置) {
        }
        
        bool CHTL词法单元::是关键字() const {
            return 类型_ >= CHTL词法单元类型::关键字_文本 && 
                   类型_ <= CHTL词法单元类型::关键字_HTML5;
        }
        
        bool CHTL词法单元::是类型标识符() const {
            return 类型_ >= CHTL词法单元类型::类型_样式 && 
                   类型_ <= CHTL词法单元类型::类型_Vue;
        }
        
        bool CHTL词法单元::是块标识符() const {
            return 类型_ >= CHTL词法单元类型::模板块 && 
                   类型_ <= CHTL词法单元类型::原始类型块;
        }
        
        bool CHTL词法单元::是字面量() const {
            return 类型_ == CHTL词法单元类型::字符串字面量 || 
                   类型_ == CHTL词法单元类型::无修饰字面量 ||
                   类型_ == CHTL词法单元类型::数字;
        }
        
        bool CHTL词法单元::是操作符() const {
            return 类型_ == CHTL词法单元类型::冒号 ||
                   类型_ == CHTL词法单元类型::等号 ||
                   类型_ == CHTL词法单元类型::箭头 ||
                   类型_ == CHTL词法单元类型::事件绑定 ||
                   类型_ == CHTL词法单元类型::引用符号;
        }
        
        bool CHTL词法单元::是注释() const {
            return 类型_ == CHTL词法单元类型::双斜杠注释 ||
                   类型_ == CHTL词法单元类型::块注释 ||
                   类型_ == CHTL词法单元类型::生成器注释;
        }
        
        bool CHTL词法单元::是HTML元素() const {
            return 类型_ == CHTL词法单元类型::HTML标签;
        }
        
        bool CHTL词法单元::是CHTL_JS语法() const {
            return 类型_ == CHTL词法单元类型::选择器开始 ||
                   类型_ == CHTL词法单元类型::选择器结束 ||
                   类型_ == CHTL词法单元类型::箭头 ||
                   类型_ == CHTL词法单元类型::事件绑定;
        }
        
        std::string CHTL词法单元::转为字符串() const {
            return 词法单元类型转字符串(类型_) + ": \"" + 值_ + "\" 位置 " +
                   std::to_string(位置_.行号) + ":" + std::to_string(位置_.列号);
        }
        
        bool CHTL词法单元::operator==(const CHTL词法单元& 其他) const {
            return 类型_ == 其他.类型_ && 值_ == 其他.值_;
        }
        
        bool CHTL词法单元::operator!=(const CHTL词法单元& 其他) const {
            return !(*this == 其他);
        }
        
        std::string 词法单元类型转字符串(CHTL词法单元类型 类型) {
            static const std::unordered_map<CHTL词法单元类型, std::string> 类型映射表 = {
                {CHTL词法单元类型::标识符, "标识符"},
                {CHTL词法单元类型::字符串字面量, "字符串字面量"},
                {CHTL词法单元类型::无修饰字面量, "无修饰字面量"},
                {CHTL词法单元类型::数字, "数字"},
                {CHTL词法单元类型::冒号, "冒号"},
                {CHTL词法单元类型::分号, "分号"},
                {CHTL词法单元类型::逗号, "逗号"},
                {CHTL词法单元类型::点号, "点号"},
                {CHTL词法单元类型::等号, "等号"},
                {CHTL词法单元类型::左大括号, "左大括号"},
                {CHTL词法单元类型::右大括号, "右大括号"},
                {CHTL词法单元类型::左中括号, "左中括号"},
                {CHTL词法单元类型::右中括号, "右中括号"},
                {CHTL词法单元类型::左小括号, "左小括号"},
                {CHTL词法单元类型::右小括号, "右小括号"},
                {CHTL词法单元类型::双斜杠注释, "双斜杠注释"},
                {CHTL词法单元类型::块注释, "块注释"},
                {CHTL词法单元类型::生成器注释, "生成器注释"},
                {CHTL词法单元类型::关键字_文本, "关键字_文本"},
                {CHTL词法单元类型::关键字_样式, "关键字_样式"},
                {CHTL词法单元类型::关键字_脚本, "关键字_脚本"},
                {CHTL词法单元类型::关键字_继承, "关键字_继承"},
                {CHTL词法单元类型::关键字_删除, "关键字_删除"},
                {CHTL词法单元类型::关键字_插入, "关键字_插入"},
                {CHTL词法单元类型::关键字_之后, "关键字_之后"},
                {CHTL词法单元类型::关键字_之前, "关键字_之前"},
                {CHTL词法单元类型::关键字_替换, "关键字_替换"},
                {CHTL词法单元类型::关键字_顶部, "关键字_顶部"},
                {CHTL词法单元类型::关键字_底部, "关键字_底部"},
                {CHTL词法单元类型::关键字_来自, "关键字_来自"},
                {CHTL词法单元类型::关键字_作为, "关键字_作为"},
                {CHTL词法单元类型::关键字_除了, "关键字_除了"},
                {CHTL词法单元类型::关键字_使用, "关键字_使用"},
                {CHTL词法单元类型::关键字_HTML5, "关键字_HTML5"},
                {CHTL词法单元类型::模板块, "模板块"},
                {CHTL词法单元类型::自定义块, "自定义块"},
                {CHTL词法单元类型::原始块, "原始块"},
                {CHTL词法单元类型::导入块, "导入块"},
                {CHTL词法单元类型::命名空间块, "命名空间块"},
                {CHTL词法单元类型::配置块, "配置块"},
                {CHTL词法单元类型::信息块, "信息块"},
                {CHTL词法单元类型::导出块, "导出块"},
                {CHTL词法单元类型::名称块, "名称块"},
                {CHTL词法单元类型::原始类型块, "原始类型块"},
                {CHTL词法单元类型::类型_样式, "类型_样式"},
                {CHTL词法单元类型::类型_元素, "类型_元素"},
                {CHTL词法单元类型::类型_变量, "类型_变量"},
                {CHTL词法单元类型::类型_HTML, "类型_HTML"},
                {CHTL词法单元类型::类型_JavaScript, "类型_JavaScript"},
                {CHTL词法单元类型::类型_CHTL, "类型_CHTL"},
                {CHTL词法单元类型::类型_CJMOD, "类型_CJMOD"},
                {CHTL词法单元类型::类型_配置, "类型_配置"},
                {CHTL词法单元类型::类型_Vue, "类型_Vue"},
                {CHTL词法单元类型::选择器开始, "选择器开始"},
                {CHTL词法单元类型::选择器结束, "选择器结束"},
                {CHTL词法单元类型::引用符号, "引用符号"},
                {CHTL词法单元类型::箭头, "箭头"},
                {CHTL词法单元类型::事件绑定, "事件绑定"},
                {CHTL词法单元类型::HTML标签, "HTML标签"},
                {CHTL词法单元类型::属性名, "属性名"},
                {CHTL词法单元类型::换行符, "换行符"},
                {CHTL词法单元类型::空白字符, "空白字符"},
                {CHTL词法单元类型::文件结束, "文件结束"},
                {CHTL词法单元类型::未知字符, "未知字符"}
            };
            
            auto 迭代器 = 类型映射表.find(类型);
            return 迭代器 != 类型映射表.end() ? 迭代器->second : "未知类型";
        }
        
        bool 是关键字字符串(const std::string& 字符串) {
            static const std::unordered_set<std::string> 关键字集合 = {
                "text", "style", "script", "inherit", "delete", "insert",
                "after", "before", "replace", "at", "top", "bottom",
                "from", "as", "except", "use", "html5"
            };
            
            return 关键字集合.find(字符串) != 关键字集合.end();
        }
        
        CHTL词法单元类型 获取关键字类型(const std::string& 字符串) {
            static const std::unordered_map<std::string, CHTL词法单元类型> 关键字映射表 = {
                {"text", CHTL词法单元类型::关键字_文本},
                {"style", CHTL词法单元类型::关键字_样式},
                {"script", CHTL词法单元类型::关键字_脚本},
                {"inherit", CHTL词法单元类型::关键字_继承},
                {"delete", CHTL词法单元类型::关键字_删除},
                {"insert", CHTL词法单元类型::关键字_插入},
                {"after", CHTL词法单元类型::关键字_之后},
                {"before", CHTL词法单元类型::关键字_之前},
                {"replace", CHTL词法单元类型::关键字_替换},
                {"from", CHTL词法单元类型::关键字_来自},
                {"as", CHTL词法单元类型::关键字_作为},
                {"except", CHTL词法单元类型::关键字_除了},
                {"use", CHTL词法单元类型::关键字_使用},
                {"html5", CHTL词法单元类型::关键字_HTML5}
            };
            
            auto 迭代器 = 关键字映射表.find(字符串);
            return 迭代器 != 关键字映射表.end() ? 迭代器->second : CHTL词法单元类型::标识符;
        }
        
    } // namespace 核心
} // namespace CHTL