#include "mysql/mysql.h"
#include <mutex>
#include <queue>
#include <memory>
#include <atomic>
#include <condition_variable>

namespace CHAT::Utils::DataBaseManager {
class DBConnectionPool {
public:
    DBConnectionPool(const char* host, int port, const char* user,
        const char* password, const char* dbName, int connectionCount);

    ~DBConnectionPool();

    DBConnectionPool(const DBConnectionPool&) = delete;

    DBConnectionPool& operator=(const DBConnectionPool&) = delete;

    std::shared_ptr<MYSQL> getConnection();
private:
    void releaseConnection(MYSQL* conn);

private:
    int m_connectionCount;
    std::queue<MYSQL*> m_connectionPool;
    std::mutex m_mutex;
    std::condition_variable m_condition;
    std::atomic<bool> m_isRunning;
};    
}