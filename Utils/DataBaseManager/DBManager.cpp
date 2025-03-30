#include "mysql/mysql.h"
#include "DBManager.hpp"
#include "JsonUtils/JsonUtils.hpp"
#include <iostream>

namespace CHAT::Utils::DataBaseManager {
DBManager::DBManager(const std::string& appName) : m_appName(appName)
{
    std::string configFilePath = "/code/CHAT/configs/DataBaseConfig.json";
    Utils::Json::JsonValue config = Utils::Json::JsonUtils::loadFromFile(configFilePath);
    const char* host = config["server"]["host"].asCString();
    int port = config["server"]["port"].asInt();
    const char* dbUser = config["database"]["user"].asCString();
    const char* dbPassword = config["database"]["password"].asCString();
    const char* dbName = config["database"]["name"].asCString();
    bool enableLogging = config["features"]["enable_logging"].asBool();
    int maxConnections = config["features"]["max_connections"].asInt();
    int maxThreadNumber = config["features"]["max_threadNumber"].asInt();
    int defaultConnections = config["features"]["default_connections"].asInt();
    int defaultThreadNumber = config["features"]["default_threadNumber"].asInt();
    try {
        m_connectionPool = std::make_unique<DBConnectionPool>(
            host, port, dbUser, dbPassword, dbName, defaultConnections);

        m_threadPool = std::make_unique<DBThreadPool>(defaultThreadNumber);
        
        m_threadPool->start();

    } catch (const std::exception& e) {
        std::cerr << "Error initializing DBManager: " << e.what() << std::endl;
        throw std::runtime_error("Failed to initialize DBManager: " + std::string(e.what()));
    }
}

bool DBManager::excuteQuery(const std::string& sql, std::function<void(MYSQL_RES*)> callback)
{
    if (!m_threadPool || !m_connectionPool) {
        throw std::runtime_error("DBManager not initialized properly");
    }

    auto task = [this, sql, callback]() {
        try {
            auto conn = m_connectionPool->getConnection();

            if (mysql_query(conn.get(), sql.c_str())) { 
                std::cerr << "SQL Error: " << mysql_error(conn.get()) << std::endl;
                return false;
            } else {
                std::cout << "Query executed: " << sql << std::endl;
            }
            MYSQL_RES* res = mysql_store_result(conn.get());
            if (callback) {
                callback(res);
            }
            if (res) {
                mysql_free_result(res);
            }
        } catch (const std::exception& e) {
            std::cerr << "Exception in SQL execution: " << e.what() << std::endl;
        }
    };
    m_threadPool->addTask(task);
    return true;
}

DBManager::~DBManager()
{
    if (m_threadPool) {
        m_threadPool->stop();
    }
}
}