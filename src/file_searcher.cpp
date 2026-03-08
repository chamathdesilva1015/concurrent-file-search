#include "file_searcher.h"
#include <fstream>
#include <iostream>
#include <algorithm>

// Thread Pool Implementation

ThreadPool::ThreadPool(size_t num_threads) : stop(false), active_tasks(0) {
    for (size_t i = 0; i < num_threads; ++i) {
        workers.emplace_back([this] {
            while (true) {
                std::function<void()> task;
                
                {
                    std::unique_lock<std::mutex> lock(queue_mutex);
                    condition.wait(lock, [this] { 
                        return stop.load() || !tasks.empty(); 
                    });
                    
                    if (stop.load() && tasks.empty()) {
                        return;
                    }
                    
                    task = std::move(tasks.front());
                    tasks.pop();
                    active_tasks++;
                }
                
                task();
                
                {
                    std::unique_lock<std::mutex> lock(queue_mutex);
                    active_tasks--;
                    finished_condition.notify_all();
                }
            }
        });
    }
}

ThreadPool::~ThreadPool() {
    stop.store(true);
    condition.notify_all();
    
    for (std::thread& worker : workers) {
        if (worker.joinable()) {
            worker.join();
        }
    }
}

void ThreadPool::enqueue(std::function<void()> task) {
    {
        std::unique_lock<std::mutex> lock(queue_mutex);
        tasks.push(std::move(task));
    }
    condition.notify_one();
}

void ThreadPool::wait_all() {
    std::unique_lock<std::mutex> lock(queue_mutex);
    finished_condition.wait(lock, [this] {
        return tasks.empty() && active_tasks.load() == 0;
    });
}

// FileSearcher Implementation

std::vector<SearchResult> FileSearcher::search_single_threaded(
    const std::string& directory,
    const std::string& keyword,
    const std::string& extension,
    bool case_sensitive
) {
    std::vector<SearchResult> results;
    
    std::string search_keyword = keyword;
    if (!case_sensitive) {
        std::transform(search_keyword.begin(), search_keyword.end(), 
                      search_keyword.begin(), ::tolower);
    }
    
    try {
        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                if (!extension.empty() && entry.path().extension() != extension) {
                    continue;
                }
                search_file(entry.path().string(), search_keyword, results, case_sensitive);
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
    
    return results;
}

std::vector<SearchResult> FileSearcher::search_multi_threaded(
    const std::string& directory,
    const std::string& keyword,
    size_t num_threads,
    const std::string& extension,
    bool case_sensitive
) {
    std::vector<SearchResult> results;
    std::mutex results_mutex;
    
    if (num_threads == 0) num_threads = 1;

    std::string search_keyword = keyword;
    if (!case_sensitive) {
        std::transform(search_keyword.begin(), search_keyword.end(), 
                      search_keyword.begin(), ::tolower);
    }
    
    ThreadPool pool(num_threads);
    
    try {
        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
            if (entry.is_regular_file()) {
                if (!extension.empty() && entry.path().extension() != extension) {
                    continue;
                }
                std::string filepath = entry.path().string();
                
                pool.enqueue([this, filepath, search_keyword, &results, &results_mutex, case_sensitive] {
                    search_file_thread_safe(filepath, search_keyword, results, results_mutex, case_sensitive);
                });
            }
        }
    } catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }
    
    pool.wait_all();
    
    return results;
}

void FileSearcher::search_file(
    const std::string& filepath,
    const std::string& keyword,
    std::vector<SearchResult>& results,
    bool case_sensitive
) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        return;
    }
    
    std::string line;
    int line_number = 0;
    
    while (std::getline(file, line)) {
        line_number++;
        
        if (case_sensitive) {
            if (line.find(keyword) != std::string::npos) {
                results.push_back({filepath, line_number, line});
            }
        } else {
            std::string line_lower = line;
            std::transform(line_lower.begin(), line_lower.end(), 
                          line_lower.begin(), ::tolower);
            
            if (line_lower.find(keyword) != std::string::npos) {
                results.push_back({filepath, line_number, line});
            }
        }
    }
    
    file.close();
}

void FileSearcher::search_file_thread_safe(
    const std::string& filepath,
    const std::string& keyword,
    std::vector<SearchResult>& results,
    std::mutex& results_mutex,
    bool case_sensitive
) {
    std::vector<SearchResult> local_results;
    search_file(filepath, keyword, local_results, case_sensitive);
    
    if (!local_results.empty()) {
        std::lock_guard<std::mutex> lock(results_mutex);
        results.insert(results.end(), local_results.begin(), local_results.end());
    }
}