#include <iostream>
#include <string>
#include <future>
#include <mutex>
#include <chrono>
#include <queue>

class KeepAlive
{
public:
    KeepAlive()
        : taskDelay(10),
        startTime(std::chrono::steady_clock::now()) // only used for debugging
    {
    }

    void queue(const std::string& value)
    {
        std::unique_lock< std::mutex > lock(mutex);
        std::cout << "queuing '" << value << " at " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count() << "ms\n";
        work.push(value);
        // increase the time to process the queue to "now + 4 seconds"

        timeout = std::chrono::steady_clock::now() + taskDelay;
        if (!running)
        {
            // launch a new asynchronous task which will process the queue
            task = std::async(std::launch::async, [this] { processWork(); });
            running = true;
        }
    }

    ~KeepAlive()
    {
        std::unique_lock< std::mutex > lock(mutex);
        // stop processing the queue
        //closing = true;
        bool wasRunning = running;
        condition.notify_all();
        lock.unlock();
        if (wasRunning)
        {
            // wait for the async task to complete
            task.wait();
        }
    }

private:
    std::mutex mutex;
    std::condition_variable condition;
    std::chrono::seconds taskDelay;
    std::chrono::steady_clock::time_point timeout;
    std::queue< std::string > work;
    std::future< void > task;
    bool closing = false;
    bool running = false;
    std::chrono::steady_clock::time_point startTime;

    void processWork()
    {
        Lunia::AutoLock lock(mutex);
        // loop until std::chrono::steady_clock::now() > timeout
        auto wait = timeout - std::chrono::steady_clock::now();
        while (!closing && wait > std::chrono::seconds(0))
        {
            condition.wait_for(lock, wait);
            wait = timeout - std::chrono::steady_clock::now();
        }
        if (!closing)
        {
            std::cout << "processing queue at " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count() << "ms\n";
            while (!work.empty())
            {
                std::cout << work.front() << "\n";
                work.pop();
            }
            std::cout << std::flush;
        }
        else
        {
            std::cout << "aborting queue processing at " << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - startTime).count() << "ms with " << work.size() << " remaining items\n";
        }
        running = false;
    }
};


//template<typename T, typename Y, typename... Args>
//void Process(T(Y::* fb), const Args& ... argsLambd)
struct Test{
public:
    Test(){
    }
    void Print() {
        printf("Test called %d \n");
    }
};
template<typename Listener>
void Process(Listener& list, void(Listener::* fb)())
{

}

int main()
{
    Test obj;
    auto x = &Test::Print;
    Process(obj, x);
    printf("Done\n");
}