#include "moduleA.hpp"
#include "moduleB.hpp"
#include "DataBaseManager/DBManager.hpp"

namespace CHAT::Module {
void ModuleA::init() {
    std::cout << "ModuleA::init()" << std::endl;
    CHAT::Utils::DataBaseManager::DBManager dbManager("moduleA");
    dbManager.excuteQuery("SELECT * FROM users", [](MYSQL_RES* result) {
        if (result) {
            int num_fields = mysql_num_fields(result);
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(result))) {
                for (int i = 0; i < num_fields; i++) {
                    std::cout << (row[i] ? row[i] : "NULL") << "\t";
                }
                std::cout << std::endl;
            }
        }
    });
}

std::string ModuleA::name() const{
    return "ModuleA";
}
}
extern "C" CHAT::Utils::Module::ModuleBase* createModule() {
    return new CHAT::Module::ModuleA();
}
