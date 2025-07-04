#pragma once

#include <string>

namespace CHAT::Utils::EnvManager {
class EnvManager {
public:
    static EnvManager& getInstance();

    ~EnvManager() = default;

    EnvManager& operator=(const EnvManager&) = delete;

    std::string getEnvByStr(const std::string& env);

    std::string getGlobalConfigPath();

    std::string getLibPath();

    std::string getLogPath();

    std::string getRestServiceConfigPath();

    std::string getRedisConfigPath();
    
private:
    EnvManager() = default;
};    
}