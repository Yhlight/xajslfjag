#include "CHTLJS词法单元.h"
#include <unordered_map>
#include <unordered_set>

namespace CHTLJS {
    namespace 核心 {
        
        CHTLJS词法单元::CHTLJS词法单元(CHTLJS词法单元类型 类型, const std::string& 值, 
                                      const CHTLJS词法单元位置& 位置)
            : 类型_(类型), 值_(值), 位置_(位置) {
        }
        
        bool CHTLJS词法单元::是CHTLJS关键字() const {
            return 类型_ >= CHTLJS词法单元类型::关键字_模块 && 
                   类型_ <= CHTLJS词法单元类型::关键字_在;
        }
        
        bool CHTLJS词法单元::是字面量() const {
            return 类型_ == CHTLJS词法单元类型::字符串字面量 || 
                   类型_ == CHTLJS词法单元类型::无修饰字面量 ||
                   类型_ == CHTLJS词法单元类型::数字;
        }
        
        bool CHTLJS词法单元::是操作符() const {
            return 类型_ == CHTLJS词法单元类型::冒号 ||
                   类型_ == CHTLJS词法单元类型::等号 ||
                   类型_ == CHTLJS词法单元类型::箭头 ||
                   类型_ == CHTLJS词法单元类型::事件绑定 ||
                   类型_ == CHTLJS词法单元类型::引用符号;
        }
        
        bool CHTLJS词法单元::是注释() const {
            return 类型_ == CHTLJS词法单元类型::双斜杠注释 ||
                   类型_ == CHTLJS词法单元类型::块注释 ||
                   类型_ == CHTLJS词法单元类型::生成器注释;
        }
        
        bool CHTLJS词法单元::是选择器语法() const {
            return 类型_ == CHTLJS词法单元类型::选择器开始 ||
                   类型_ == CHTLJS词法单元类型::选择器结束 ||
                   类型_ == CHTLJS词法单元类型::引用符号;
        }
        
        bool CHTLJS词法单元::是函数语法() const {
            return 类型_ == CHTLJS词法单元类型::函数开始 ||
                   类型_ == CHTLJS词法单元类型::函数结束;
        }
        
        bool CHTLJS词法单元::是模块语法() const {
            return 类型_ == CHTLJS词法单元类型::关键字_模块 ||
                   类型_ == CHTLJS词法单元类型::关键字_加载;
        }
        
        bool CHTLJS词法单元::是动画语法() const {
            return 类型_ == CHTLJS词法单元类型::关键字_动画 ||
                   类型_ == CHTLJS词法单元类型::关键字_持续时间 ||
                   类型_ == CHTLJS词法单元类型::关键字_缓慢函数 ||
                   类型_ == CHTLJS词法单元类型::关键字_开始 ||
                   类型_ == CHTLJS词法单元类型::关键字_何时 ||
                   类型_ == CHTLJS词法单元类型::关键字_结束 ||
                   类型_ == CHTLJS词法单元类型::关键字_循环 ||
                   类型_ == CHTLJS词法单元类型::关键字_方向 ||
                   类型_ == CHTLJS词法单元类型::关键字_延迟 ||
                   类型_ == CHTLJS词法单元类型::关键字_回调 ||
                   类型_ == CHTLJS词法单元类型::关键字_在;
        }
        
        bool CHTLJS词法单元::是虚对象语法() const {
            return 类型_ == CHTLJS词法单元类型::关键字_虚对象;
        }
        
        std::string CHTLJS词法单元::转为字符串() const {
            return CHTLJS词法单元类型转字符串(类型_) + ": \"" + 值_ + "\" 位置 " +
                   std::to_string(位置_.行号) + ":" + std::to_string(位置_.列号);
        }
        
        bool CHTLJS词法单元::operator==(const CHTLJS词法单元& 其他) const {
            return 类型_ == 其他.类型_ && 值_ == 其他.值_;
        }
        
        bool CHTLJS词法单元::operator!=(const CHTLJS词法单元& 其他) const {
            return !(*this == 其他);
        }
        
        std::string CHTLJS词法单元类型转字符串(CHTLJS词法单元类型 类型) {
            static const std::unordered_map<CHTLJS词法单元类型, std::string> 类型映射表 = {
                {CHTLJS词法单元类型::标识符, "CHTLJS_标识符"},
                {CHTLJS词法单元类型::字符串字面量, "CHTLJS_字符串字面量"},
                {CHTLJS词法单元类型::无修饰字面量, "CHTLJS_无修饰字面量"},
                {CHTLJS词法单元类型::数字, "CHTLJS_数字"},
                {CHTLJS词法单元类型::冒号, "CHTLJS_冒号"},
                {CHTLJS词法单元类型::分号, "CHTLJS_分号"},
                {CHTLJS词法单元类型::逗号, "CHTLJS_逗号"},
                {CHTLJS词法单元类型::点号, "CHTLJS_点号"},
                {CHTLJS词法单元类型::等号, "CHTLJS_等号"},
                {CHTLJS词法单元类型::左大括号, "CHTLJS_左大括号"},
                {CHTLJS词法单元类型::右大括号, "CHTLJS_右大括号"},
                {CHTLJS词法单元类型::左中括号, "CHTLJS_左中括号"},
                {CHTLJS词法单元类型::右中括号, "CHTLJS_右中括号"},
                {CHTLJS词法单元类型::左小括号, "CHTLJS_左小括号"},
                {CHTLJS词法单元类型::右小括号, "CHTLJS_右小括号"},
                {CHTLJS词法单元类型::双斜杠注释, "CHTLJS_双斜杠注释"},
                {CHTLJS词法单元类型::块注释, "CHTLJS_块注释"},
                {CHTLJS词法单元类型::生成器注释, "CHTLJS_生成器注释"},
                {CHTLJS词法单元类型::关键字_模块, "CHTLJS_关键字_模块"},
                {CHTLJS词法单元类型::关键字_加载, "CHTLJS_关键字_加载"},
                {CHTLJS词法单元类型::关键字_监听, "CHTLJS_关键字_监听"},
                {CHTLJS词法单元类型::关键字_委托, "CHTLJS_关键字_委托"},
                {CHTLJS词法单元类型::关键字_目标, "CHTLJS_关键字_目标"},
                {CHTLJS词法单元类型::关键字_动画, "CHTLJS_关键字_动画"},
                {CHTLJS词法单元类型::关键字_持续时间, "CHTLJS_关键字_持续时间"},
                {CHTLJS词法单元类型::关键字_缓慢函数, "CHTLJS_关键字_缓慢函数"},
                {CHTLJS词法单元类型::关键字_开始, "CHTLJS_关键字_开始"},
                {CHTLJS词法单元类型::关键字_何时, "CHTLJS_关键字_何时"},
                {CHTLJS词法单元类型::关键字_结束, "CHTLJS_关键字_结束"},
                {CHTLJS词法单元类型::关键字_循环, "CHTLJS_关键字_循环"},
                {CHTLJS词法单元类型::关键字_方向, "CHTLJS_关键字_方向"},
                {CHTLJS词法单元类型::关键字_延迟, "CHTLJS_关键字_延迟"},
                {CHTLJS词法单元类型::关键字_回调, "CHTLJS_关键字_回调"},
                {CHTLJS词法单元类型::关键字_虚对象, "CHTLJS_关键字_虚对象"},
                {CHTLJS词法单元类型::关键字_在, "CHTLJS_关键字_在"},
                {CHTLJS词法单元类型::选择器开始, "CHTLJS_选择器开始"},
                {CHTLJS词法单元类型::选择器结束, "CHTLJS_选择器结束"},
                {CHTLJS词法单元类型::引用符号, "CHTLJS_引用符号"},
                {CHTLJS词法单元类型::箭头, "CHTLJS_箭头"},
                {CHTLJS词法单元类型::事件绑定, "CHTLJS_事件绑定"},
                {CHTLJS词法单元类型::函数开始, "CHTLJS_函数开始"},
                {CHTLJS词法单元类型::函数结束, "CHTLJS_函数结束"},
                {CHTLJS词法单元类型::换行符, "CHTLJS_换行符"},
                {CHTLJS词法单元类型::空白字符, "CHTLJS_空白字符"},
                {CHTLJS词法单元类型::文件结束, "CHTLJS_文件结束"},
                {CHTLJS词法单元类型::未知字符, "CHTLJS_未知字符"}
            };
            
            auto 迭代器 = 类型映射表.find(类型);
            return 迭代器 != 类型映射表.end() ? 迭代器->second : "CHTLJS_未知类型";
        }
        
        bool 是CHTLJS关键字字符串(const std::string& 字符串) {
            // 严格按照CHTL语法文档中CHTL JS部分定义的关键字
            static const std::unordered_set<std::string> CHTLJS关键字集合 = {
                "module", "load", "listen", "delegate", "target", "animate",
                "duration", "easing", "begin", "when", "end", "loop",
                "direction", "delay", "callback", "vir", "at"
            };
            
            return CHTLJS关键字集合.find(字符串) != CHTLJS关键字集合.end();
        }
        
        CHTLJS词法单元类型 获取CHTLJS关键字类型(const std::string& 字符串) {
            // 严格按照CHTL语法文档中CHTL JS部分定义的关键字映射
            static const std::unordered_map<std::string, CHTLJS词法单元类型> CHTLJS关键字映射表 = {
                {"module", CHTLJS词法单元类型::关键字_模块},
                {"load", CHTLJS词法单元类型::关键字_加载},
                {"listen", CHTLJS词法单元类型::关键字_监听},
                {"delegate", CHTLJS词法单元类型::关键字_委托},
                {"target", CHTLJS词法单元类型::关键字_目标},
                {"animate", CHTLJS词法单元类型::关键字_动画},
                {"duration", CHTLJS词法单元类型::关键字_持续时间},
                {"easing", CHTLJS词法单元类型::关键字_缓慢函数},
                {"begin", CHTLJS词法单元类型::关键字_开始},
                {"when", CHTLJS词法单元类型::关键字_何时},
                {"end", CHTLJS词法单元类型::关键字_结束},
                {"loop", CHTLJS词法单元类型::关键字_循环},
                {"direction", CHTLJS词法单元类型::关键字_方向},
                {"delay", CHTLJS词法单元类型::关键字_延迟},
                {"callback", CHTLJS词法单元类型::关键字_回调},
                {"vir", CHTLJS词法单元类型::关键字_虚对象},
                {"at", CHTLJS词法单元类型::关键字_在}
            };
            
            auto 迭代器 = CHTLJS关键字映射表.find(字符串);
            return 迭代器 != CHTLJS关键字映射表.end() ? 迭代器->second : CHTLJS词法单元类型::标识符;
        }
        
    } // namespace 核心
} // namespace CHTLJS