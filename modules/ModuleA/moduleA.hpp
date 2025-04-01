#include "Module/ModuleBase.hpp"
#include <iostream>
#include <memory>
#include "mysql/mysql.h"
#include "DataBaseManager/DBManager.hpp"

namespace CHAT::Module {
class ModuleA : public CHAT::Utils::Module::ModuleBase {
public:
    ModuleA() = default;
    ~ModuleA() = default;
    void init() override;
    std::string name() const override;
    void processResult(MYSQL_RES* result);
private:
    std::unique_ptr<CHAT::Utils::DataBaseManager::DBManager> dbManager;    
};
}
