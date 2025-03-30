#include <string>
#include <memory>
#include "DBThreadPool.hpp"
#include "DBConnectionPool.hpp"
#include <functional>

namespace CHAT::Utils::DataBaseManager {
class DBManager {
public:
    DBManager(const std::string& appName);

    DBManager(const DBManager&) = delete;

    DBManager& operator=(const DBManager&) = delete;

    DBManager& operator=(DBManager&&) = delete;

    DBManager(DBManager&&) = delete;

    ~DBManager();

    bool excuteQuery(const std::string& sql, std::function<void(MYSQL_RES*)> callback);

private:
    std::string m_appName;
    std::unique_ptr<DBThreadPool> m_threadPool;
    std::unique_ptr<DBConnectionPool> m_connectionPool;
};    
}