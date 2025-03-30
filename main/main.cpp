#include "Module/ModuleLoader.hpp"
#include <iostream>
#include <vector>
#include <memory>
#include <dlfcn.h>
#include <filesystem>
#include "JsonUtils/JsonUtils.hpp"
#include "FileUtils/fileSystem.hpp"

int main() {
    CHAT::Utils::Module::ModuleLoader loader;
    CHAT::Utils::Json::JsonUtils jsonUtils;
    std::string moduleConfigPath = ("/code/CHAT/configs/ModuleDefine.json");
    CHAT::Utils::Json::JsonValue jsonValue = jsonUtils.loadFromFile(moduleConfigPath);
    std::vector<std::string> modules;
    if (jsonValue.isMember("modules") && jsonValue["modules"].isArray()) {
        for (const auto& module : jsonValue["modules"]) {
            if (module.isString()) {
                modules.push_back(module.asString());
            } else {
                std::cerr << "Module name must be string" << std::endl;
            }
        }
    } else {
        std::cerr << "ModuleDefine.json must contains modules array" << std::endl;
    }
    loader.loadModules(modules);
    return 0;
}
