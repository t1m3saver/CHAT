#include <thread>
#include <queue>
#include <functional>
#include <atomic>
#include <mutex>
#include <condition_variable>

namespace CHAT::Utils::DataBaseManager {
class DBThreadPool {
public:
    explicit DBThreadPool(const int& threadNum);

    ~DBThreadPool();

    void start();

    void stop();

    void addTask(std::function<void()> task);

private:
    int m_threadNum;
    std::vector<std::thread> m_workers;    
    std::queue<std::function<void()>> m_tasks;
    std::mutex m_queueMutex;
    std::condition_variable m_condition;
    std::atomic<bool> m_running;
};    
}