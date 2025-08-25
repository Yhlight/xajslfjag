#pragma once

#include <string>
#include <set>
#include <map>
#include <memory>
#include "Ast.h" // Assuming View is defined in Ast.h

namespace CHTL_JS {

    // Singleton registry for vir-related information
    class ViewRegistry {
    public:
        static ViewRegistry& GetInstance() {
            static ViewRegistry instance;
            return instance;
        }

        void RegisterView(const std::string& viewName, View view);
        std::shared_ptr<View> GetView(const std::string& viewName);

    private:
        ViewRegistry() = default;

        std::map<std::string, std::shared_ptr<View>> m_Views;
    };

}
