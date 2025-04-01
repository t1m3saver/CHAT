#include <string>
#include <memory>
#include "DBThreadPool.hpp"
#include "DBConnectionPool.hpp"
#include <functional>
#include <future>

namespace CHAT::Utils::DataBaseManager {
class DBManager {
public:
    DBManager(const std::string& appName);

    DBManager(const DBManager&) = delete;

    DBManager& operator=(const DBManager&) = delete;

    DBManager& operator=(DBManager&&) = delete;

    DBManager(DBManager&&) = delete;

    ~DBManager();

    std::future<std::shared_ptr<MYSQL_RES>> excuteQuery(const std::string& sql);

private:
    std::string m_appName;
    std::unique_ptr<DBThreadPool> m_threadPool;
    std::unique_ptr<DBConnectionPool> m_connectionPool;
};    
}