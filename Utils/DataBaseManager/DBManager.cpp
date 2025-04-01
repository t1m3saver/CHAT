#include "mysql/mysql.h"
#include "DBManager.hpp"
#include "JsonUtils/JsonUtils.hpp"
#include <iostream>
#include <future>

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

        m_threadPool = std::make_unique<DBThreadPool>(defaultThreadNumber);

        m_connectionPool = std::make_unique<DBConnectionPool>(
            host, port, dbUser, dbPassword, dbName, defaultConnections);
        
        m_threadPool->start();

    } catch (const std::exception& e) {
        std::cerr << "Error initializing DBManager: " << e.what() << std::endl;
        throw std::runtime_error("Failed to initialize DBManager: " + std::string(e.what()));
    }
}

std::future<std::shared_ptr<MYSQL_RES>> DBManager::excuteQuery(const std::string& sql)
{
    if (!m_threadPool || !m_connectionPool) {
        throw std::runtime_error("DBManager not initialized properly");
    }

    auto promise = std::make_shared<std::promise<std::shared_ptr<MYSQL_RES>>>();
    auto task = [this, sql, promise]() {
        try {
            auto conn = m_connectionPool->getConnection();
            if (mysql_query(conn.get(), sql.c_str())) { 
                promise->set_value(nullptr);
                std::cerr << "SQL Error: " << mysql_error(conn.get()) << std::endl;
                return;
            }
            std::cout << "Query executed: " << sql << std::endl;

            MYSQL_RES* raw_res = mysql_store_result(conn.get());
            if (!raw_res) {
                promise->set_value(nullptr);
                std::cerr << "Error storing result: " << mysql_error(conn.get()) << std::endl;
                return;
            }
            auto res = std::shared_ptr<MYSQL_RES>(raw_res, [](MYSQL_RES* r) { mysql_free_result(r); });
            promise->set_value(res);

        } catch (const std::exception& e) {
            promise->set_value(nullptr);
            std::cerr << "Exception in SQL execution: " << e.what() << std::endl;
        }
    };

    m_threadPool->addTask(std::move(task));
    return promise->get_future();
}


DBManager::~DBManager()
{
    std::cout << "Destroying DBManager, cleaning up resources" << std::endl;
    if (m_threadPool) {
        m_threadPool->stop();
        std::cout << "Thread pool stopped." << std::endl;
    }

    if (m_connectionPool) {
        std::cout << "Destroying connection pool" << std::endl;
        m_connectionPool.reset(); 
    }
}
}