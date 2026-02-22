# Concurrent File Search Tool

A high-performance C++ application that recursively searches directories for keywords using multithreading and thread pool optimization.

## Features

- **Recursive Directory Search**: Traverses entire directory trees
- **Case-Insensitive Matching**: Finds keywords regardless of capitalization
- **Thread Pool Implementation**: Custom thread pool for efficient parallel processing
- **Performance Benchmarking**: Built-in comparison between single and multi-threaded execution
- **Scalable Architecture**: Automatically utilizes all available CPU cores

## Performance

Tested on 2,000 files (1 million lines of text):

| Method | Time | Speedup |
|--------|------|---------|
| Single-threaded | 1114ms | 1.0x |
| Multi-threaded (10 threads) | 168ms | **6.63x** |

*Results may vary based on hardware and dataset*

## Technical Highlights

- **Thread Pool Pattern**: Custom implementation with work queue and condition variables
- **Thread Safety**: Mutex-protected result aggregation
- **Modern C++17**: Uses `std::filesystem`, `std::atomic`, and STL containers
- **Resource Management**: RAII-compliant thread pool with proper cleanup
- **Cross-platform**: Works on Linux, macOS, and Windows

## Prerequisites

- C++17 compatible compiler (g++ 7.0+, clang 5.0+, MSVC 2017+)
- Make (optional, for build automation)

## Building
```bash
# Clone the repository
git clone https://github.com/yourusername/concurrent-file-search.git
cd concurrent-file-search

# Build the project
make

# Generate test data (optional)
./generate_test_data 2000 500
```

## Usage
```bash
./file_search <directory> <keyword>
```

**Example:**
```bash
./file_search ./large_test_data TARGET_KEYWORD
```

**Output:**
```
========================================
Concurrent File Search Benchmark
========================================

✓ Single-Threaded Search
  Matches: 1363
  Time: 1114ms

✓ Multi-Threaded Search (10 threads)
  Matches: 1363
  Time: 168ms

========================================
⚡ Speedup: 6.63x faster
========================================
```

## Architecture

### Components

1. **FileSearcher Class**: Main search logic with single and multi-threaded implementations
2. **ThreadPool Class**: Custom thread pool managing worker threads and task queue
3. **SearchResult Struct**: Stores match information (filepath, line number, content)

### Thread Pool Design
```cpp
class ThreadPool {
    - Worker threads spawned at initialization
    - Task queue with mutex protection
    - Condition variables for thread synchronization
    - Wait mechanism to ensure all tasks complete
}
```

## Testing

Generate test data with custom parameters:
```bash
# Generate 5000 files with 1000 lines each
./generate_test_data 5000 1000
```

## Future Enhancements

- [ ] Add regex pattern matching
- [ ] Implement file type filtering
- [ ] Add progress bar for large searches
- [ ] Support for binary file detection
- [ ] Export results to JSON/CSV
- [ ] Add memory-mapped file I/O for larger files

## Learning Outcomes

This project demonstrates:
- Concurrent programming with C++ threads
- Synchronization primitives (mutexes, condition variables, atomics)
- Thread pool design pattern
- Performance optimization techniques
- File I/O and directory traversal
- Modern C++ best practices

## License

MIT License - feel free to use for learning and projects

## 👤 Author

Chamath De Silva
- GitHub: chamathdesilva1015 

*Built as a portfolio project to demonstrate systems programming and concurrency skills*