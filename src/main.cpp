#include "file_searcher.h"
#include <iostream>
#include <chrono>
#include <iomanip>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cout << "Usage: " << argv[0] << " <directory> <keyword>" << std::endl;
        return 1;
    }
    
    std::string directory = argv[1];
    std::string keyword = argv[2];
    
    FileSearcher searcher;
    
    std::cout << "\n========================================" << std::endl;
    std::cout << "Concurrent File Search Benchmark" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // Single-threaded search
    auto start_single = std::chrono::high_resolution_clock::now();
    auto results_single = searcher.search_single_threaded(directory, keyword);
    auto end_single = std::chrono::high_resolution_clock::now();
    auto duration_single = std::chrono::duration_cast<std::chrono::milliseconds>(end_single - start_single);
    
    std::cout << "✓ Single-Threaded Search" << std::endl;
    std::cout << "  Matches: " << results_single.size() << std::endl;
    std::cout << "  Time: " << duration_single.count() << "ms\n" << std::endl;
    
    // Multi-threaded search
    size_t num_threads = std::thread::hardware_concurrency();
    auto start_multi = std::chrono::high_resolution_clock::now();
    auto results_multi = searcher.search_multi_threaded(directory, keyword, num_threads);
    auto end_multi = std::chrono::high_resolution_clock::now();
    auto duration_multi = std::chrono::duration_cast<std::chrono::milliseconds>(end_multi - start_multi);
    
    std::cout << "✓ Multi-Threaded Search (" << num_threads << " threads)" << std::endl;
    std::cout << "  Matches: " << results_multi.size() << std::endl;
    std::cout << "  Time: " << duration_multi.count() << "ms\n" << std::endl;
    
    // Calculate speedup
    double speedup = static_cast<double>(duration_single.count()) / duration_multi.count();
    std::cout << "========================================" << std::endl;
    std::cout << "⚡ Speedup: " << std::fixed << std::setprecision(2) 
              << speedup << "x faster" << std::endl;
    std::cout << "========================================\n" << std::endl;
    
    // Display first 10 results
    std::cout << "Sample Results (first 10):" << std::endl;
    for (size_t i = 0; i < std::min(results_multi.size(), size_t(10)); ++i) {
        std::cout << "  " << results_multi[i].filepath << ":" 
                  << results_multi[i].line_number << std::endl;
    }
    
    return 0;
}