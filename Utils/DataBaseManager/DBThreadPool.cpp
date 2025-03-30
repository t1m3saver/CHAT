#include "DBThreadPool.hpp"
#include <iostream>

namespace CHAT::Utils::DataBaseManager {
DBThreadPool::DBThreadPool(const int& threadNum)
{
    m_threadNum = threadNum;
}

DBThreadPool::~DBThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_running = false;
    }
    for (auto& thread : m_workers) {
        thread.join();
    }  
}

void DBThreadPool::stop()
{
    std::unique_lock<std::mutex> lock(m_queueMutex);
    m_running = false;
    m_condition.notify_all(); 
}

void DBThreadPool::start()
{
    for (int i = 0; i < m_threadNum; ++i) {
        m_workers.emplace_back([this, i] {
            while (true) {
                std::function<void()> task;
                {
                    std::unique_lock<std::mutex> lock(m_queueMutex);
                    m_condition.wait(lock, [this] { return !m_running || !m_tasks.empty(); });
                    if (!m_running && m_tasks.empty()) {
                        return;
                    }
                    task = std::move(m_tasks.front());
                    m_tasks.pop();
                }
                try {
                    std::cout << "Thread " << std::this_thread::get_id() << " executing task. id is " << i + 1 << std::endl;
                    task();  
                } catch (const std::exception& e) {
                    std::cerr << "Exception during task execution: " << e.what() << std::endl;
                }
            }
        });
    }
    std::cout << "Thread pool started with " << m_workers.size() << " threads." << std::endl;
    m_running = true;
}

void DBThreadPool::addTask(std::function<void()> task)
{
    {
        std::unique_lock<std::mutex> lock(m_queueMutex);
        m_tasks.push(std::move(task));
    }
    m_condition.notify_one();
}
}