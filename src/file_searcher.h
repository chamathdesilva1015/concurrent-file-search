#ifndef FILE_SEARCHER_H
#define FILE_SEARCHER_H

#if __cplusplus < 201703L
#error "This project requires C++17 or later. Please use -std=c++17 or higher."
#endif

#include <string>
#include <vector>
#include <filesystem>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <atomic>

namespace fs = std::filesystem;

struct SearchResult {
    std::string filepath;
    int line_number;
    std::string line_content;
};

// Thread Pool for managing worker threads
class ThreadPool {
public:
    ThreadPool(size_t num_threads);
    ~ThreadPool();
    
    void enqueue(std::function<void()> task);
    void wait_all();

private:
    std::vector<std::thread> workers;
    std::queue<std::function<void()>> tasks;
    
    std::mutex queue_mutex;
    std::condition_variable condition;
    std::atomic<bool> stop;
    std::atomic<int> active_tasks;
    std::condition_variable finished_condition;
};

class FileSearcher {
public:
    std::vector<SearchResult> search_single_threaded(
        const std::string& directory,
        const std::string& keyword,
        const std::string& extension = "",
        bool case_sensitive = false
    );
    
    std::vector<SearchResult> search_multi_threaded(
        const std::string& directory,
        const std::string& keyword,
        size_t num_threads = std::thread::hardware_concurrency(),
        const std::string& extension = "",
        bool case_sensitive = false
    );

private:
    void search_file(
        const std::string& filepath,
        const std::string& keyword,
        std::vector<SearchResult>& results,
        bool case_sensitive = false
    );
    
    void search_file_thread_safe(
        const std::string& filepath,
        const std::string& keyword,
        std::vector<SearchResult>& results,
        std::mutex& results_mutex,
        bool case_sensitive = false
    );
};

#endif