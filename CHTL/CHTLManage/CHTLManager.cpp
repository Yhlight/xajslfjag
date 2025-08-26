#include "CHTLManager.h"
#include <fstream>
#include <iostream>

namespace CHTL {

CHTLManager::CHTLManager() {
    lexer = std::make_unique<CHTLLexer>("");
    parser = std::make_unique<CHTLParser>();
    generator = std::make_unique<CHTLGenerator>();
    context = std::make_unique<CHTLContext>();
    global_map = std::make_unique<GlobalMap>();
    loader = std::make_unique<CHTLLoader>();
    
    generator->setContext(context.get());
}

CHTLManager::CompilationResult CHTLManager::compileFile(const std::string& filepath) {
    CompilationResult result;
    result.success = false;
    
    try {
        std::string source = loader->loadFile(filepath);
        if (source.empty()) {
            result.errors.push_back("Failed to load file: " + filepath);
            return result;
        }
        
        return compileString(source, loader->getFileDirectory(filepath));
        
    } catch (const std::exception& e) {
        result.errors.push_back("File compilation error: " + std::string(e.what()));
    }
    
    return result;
}

CHTLManager::CompilationResult CHTLManager::compileString(const std::string& source, const std::string& context_path) {
    CompilationResult result;
    result.success = false;
    
    try {
        // Clear previous state
        errors.clear();
        warnings.clear();
        
        // Set up context
        if (!context_path.empty()) {
            loader->addIncludePath(context_path);
        }
        
        // Tokenize
        lexer->reset(source);
        auto tokens = lexer->tokenize();
        
        if (lexer->hasErrors()) {
            for (const auto& error : lexer->getErrors()) {
                errors.push_back(error);
            }
        }
        
        if (!tokens.empty()) {
            // Parse
            auto ast = parser->parse(tokens);
            
            if (parser->hasErrors()) {
                for (const auto& error : parser->getErrors()) {
                    errors.push_back(error);
                }
            }
            
            if (ast) {
                // Collect definitions
                collectDefinitions(ast);
                
                // Process imports
                processImports(ast);
                
                // Validate AST
                validateAST(ast);
                
                // Generate code
                generator->setContext(context.get());
                generator->setPrettyPrint(config.pretty_print);
                
                auto generated = generator->generate(ast);
                
                result.html = generated.html;
                result.css = generated.css;
                result.js = generated.js;
                result.success = errors.empty();
            }
        }
        
        result.errors = errors;
        result.warnings = warnings;
        
    } catch (const std::exception& e) {
        result.errors.push_back("Compilation error: " + std::string(e.what()));
    }
    
    return result;
}

std::vector<CHTLManager::CompilationResult> CHTLManager::compileDirectory(const std::string& directory, const std::string& pattern) {
    std::vector<CompilationResult> results;
    
    auto files = loader->findFilesInDirectory(directory, pattern);
    
    for (const auto& file : files) {
        auto result = compileFile(file);
        results.push_back(result);
    }
    
    return results;
}

void CHTLManager::setConfig(const std::string& key, const std::string& value) {
    context->setConfig(key, value);
    
    if (key == "pretty_print") {
        config.pretty_print = (value == "true");
    } else if (key == "debug_mode") {
        config.debug_mode = (value == "true");
    } else if (key == "strict_mode") {
        config.strict_mode = (value == "true");
    } else if (key == "output_dir") {
        config.output_dir = value;
    }
}

std::string CHTLManager::getConfig(const std::string& key) const {
    return context->getConfig(key);
}

void CHTLManager::loadConfigFile(const std::string& config_path) {
    std::ifstream file(config_path);
    if (!file.is_open()) {
        errors.push_back("Cannot open config file: " + config_path);
        return;
    }
    
    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            setConfig(key, value);
        }
    }
}

bool CHTLManager::loadModule(const std::string& module_name) {
    if (isModuleLoaded(module_name)) {
        return true;
    }
    
    std::string module_content = loader->loadModule(module_name);
    if (module_content.empty()) {
        errors.push_back("Failed to load module: " + module_name);
        return false;
    }
    
    // Parse and register module content
    auto result = compileString(module_content);
    if (result.success) {
        loaded_modules.insert(module_name);
        return true;
    }
    
    return false;
}

bool CHTLManager::isModuleLoaded(const std::string& module_name) const {
    return loaded_modules.find(module_name) != loaded_modules.end();
}

void CHTLManager::addModulePath(const std::string& name, const std::string& path) {
    module_paths[name] = path;
    loader->addIncludePath(path);
}

void CHTLManager::registerTemplate(const std::string& name, std::shared_ptr<CHTLNode> template_node) {
    templates[name] = template_node;
}

void CHTLManager::registerCustom(const std::string& name, std::shared_ptr<CHTLNode> custom_node) {
    customs[name] = custom_node;
}

void CHTLManager::registerVariable(const std::string& name, const std::string& value) {
    variables[name] = value;
}

std::shared_ptr<CHTLNode> CHTLManager::getTemplate(const std::string& name) const {
    auto it = templates.find(name);
    return (it != templates.end()) ? it->second : nullptr;
}

std::shared_ptr<CHTLNode> CHTLManager::getCustom(const std::string& name) const {
    auto it = customs.find(name);
    return (it != customs.end()) ? it->second : nullptr;
}

std::string CHTLManager::getVariable(const std::string& name) const {
    auto it = variables.find(name);
    return (it != variables.end()) ? it->second : "";
}

void CHTLManager::collectDefinitions(std::shared_ptr<CHTLNode> ast) {
    if (!ast) return;
    
    ast->traverse([this](CHTLNode* node) {
        if (node->type == NodeType::TEMPLATE_DEFINITION) {
            registerTemplate(node->name, std::shared_ptr<CHTLNode>(node));
        } else if (node->type == NodeType::CUSTOM_DEFINITION) {
            registerCustom(node->name, std::shared_ptr<CHTLNode>(node));
        }
    });
}

void CHTLManager::processImports(std::shared_ptr<CHTLNode> ast) {
    if (!ast) return;
    
    ast->traverse([this](CHTLNode* node) {
        if (node->type == NodeType::IMPORT_STATEMENT) {
            // Process import
            std::string import_path = node->getAttribute("path");
            if (!import_path.empty()) {
                loadModule(import_path);
            }
        }
    });
}

void CHTLManager::validateAST(std::shared_ptr<CHTLNode> ast) {
    if (!ast) return;
    
    // Basic validation
    ast->traverse([this](CHTLNode* node) {
        if (node->type == NodeType::HTML_ELEMENT) {
            if (node->name.empty()) {
                warnings.push_back("Empty element name");
            }
        }
    });
}

void CHTLManager::reset() {
    errors.clear();
    warnings.clear();
    templates.clear();
    customs.clear();
    variables.clear();
    loaded_modules.clear();
    
    if (parser) parser->reset();
    if (context) context->clear();
    if (global_map) global_map->clear();
    if (loader) loader->clearLoadedFiles();
}

void CHTLManager::printStatistics() const {
    std::cout << "CHTL Manager Statistics:" << std::endl;
    std::cout << "Templates: " << templates.size() << std::endl;
    std::cout << "Customs: " << customs.size() << std::endl;
    std::cout << "Variables: " << variables.size() << std::endl;
    std::cout << "Loaded Modules: " << loaded_modules.size() << std::endl;
    std::cout << "Errors: " << errors.size() << std::endl;
    std::cout << "Warnings: " << warnings.size() << std::endl;
}

bool CHTLManager::saveToFile(const std::string& filepath, const std::string& content) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        return false;
    }
    
    file << content;
    file.close();
    return true;
}

bool CHTLManager::saveCompilationResult(const CompilationResult& result, const std::string& base_path) const {
    if (!result.success) {
        return false;
    }
    
    bool all_saved = true;
    
    if (!result.html.empty()) {
        all_saved &= saveToFile(base_path + ".html", result.html);
    }
    
    if (!result.css.empty()) {
        all_saved &= saveToFile(base_path + ".css", result.css);
    }
    
    if (!result.js.empty()) {
        all_saved &= saveToFile(base_path + ".js", result.js);
    }
    
    return all_saved;
}

} // namespace CHTL