#include "DBConnectionPool.hpp"
#include <mutex>
#include <iostream>

namespace CHAT::Utils::DataBaseManager {
DBConnectionPool::DBConnectionPool(const char* host, int port, const char* user,
    const char* password, const char* dbName, int connectionCount)
{
    m_connectionCount = connectionCount;
    std::lock_guard<std::mutex> lock(m_mutex);
    try {
        for (int i = 0; i < connectionCount; ++i) {
            MYSQL* conn = mysql_init(nullptr);
            if (conn == nullptr) {
                throw std::runtime_error("MySQL init failed");
            }
            if (!mysql_real_connect(conn, host, user, password, dbName, port, nullptr, 0)) {
                mysql_close(conn);
                throw std::runtime_error("MySQL connect failed: " + std::string(mysql_error(conn)));
            }
            std::cout << "MySQL connection established: " << i + 1 << std::endl;
            m_connectionPool.push(conn);
        }
    } catch (...) {
        while (!m_connectionPool.empty()) {
            mysql_close(m_connectionPool.front());
            m_connectionPool.pop();
        }
        throw;
    }
    std::cout << "MySQL connection pool initialized with " << m_connectionPool.size() << " connections." << std::endl;
    m_isRunning = true;
}
DBConnectionPool::~DBConnectionPool()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    m_isRunning = false;
    while (!m_connectionPool.empty()) {
        MYSQL* conn = m_connectionPool.front();
        m_connectionPool.pop();
        mysql_close(conn);
    }
    m_condition.notify_all(); 
}

std::shared_ptr<MYSQL> DBConnectionPool::getConnection()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    
    bool gotConnection = m_condition.wait_for(lock, std::chrono::seconds(5), [this]() {
        return !m_connectionPool.empty() || !m_isRunning;
    });

    if (!gotConnection) {
        throw std::runtime_error("Timeout 5 seconds: No available database connections.");
    }

    if (!m_isRunning) {
        throw std::runtime_error("DBConnectionPool is stopping, no connections available.");
    }

    MYSQL* conn = m_connectionPool.front();
    m_connectionPool.pop();
    return std::shared_ptr<MYSQL>(conn, [this](MYSQL* c) { releaseConnection(c); });
}

void DBConnectionPool::releaseConnection(MYSQL* conn) 
{
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_isRunning) {
        m_connectionPool.push(conn);
        m_condition.notify_one();
    } else {
        mysql_close(conn);
    }
}
}
