#include "moduleA.hpp"
#include "moduleB.hpp"

namespace CHAT::Module {
void ModuleA::init() {
    std::cout << "ModuleA::init()" << std::endl;
    dbManager = std::make_unique<CHAT::Utils::DataBaseManager::DBManager>("a");
    for(int i = 0; i < 10; ++i) {
        std::future<std::shared_ptr<MYSQL_RES>> futureRes = dbManager->excuteQuery("SELECT * FROM user");
        std::shared_ptr<MYSQL_RES> res = futureRes.get(); 
        if (res) {
            MYSQL_ROW row;
            while ((row = mysql_fetch_row(res.get()))) {
                std::cout << row[0] << std::endl;
            }
        }        
    }

}

std::string ModuleA::name() const{
    return "ModuleA";
}

}
extern "C" CHAT::Utils::Module::ModuleBase* createModule() {
    return new CHAT::Module::ModuleA();
}
