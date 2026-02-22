#include <iostream>
#include <fstream>
#include <filesystem>
#include <random>
#include <vector>

namespace fs = std::filesystem;

std::vector<std::string> words = {
    "hello", "world", "search", "concurrent", "thread", "file", "data",
    "algorithm", "performance", "optimization", "parallel", "computing",
    "system", "programming", "software", "engineering", "development"
};

std::string generate_line(std::mt19937& gen, std::uniform_int_distribution<>& dist, bool include_keyword) {
    std::string line;
    int word_count = 10 + (dist(gen) % 20);
    
    for (int i = 0; i < word_count; ++i) {
        if (include_keyword && i == word_count / 2) {
            line += "TARGET_KEYWORD ";
        } else {
            line += words[dist(gen) % words.size()] + " ";
        }
    }
    return line;
}

void create_test_files(const std::string& base_dir, int num_files, int lines_per_file) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dist(0, 1000);
    
    fs::create_directories(base_dir);
    
    std::cout << "Generating " << num_files << " test files..." << std::endl;
    
    for (int file_num = 0; file_num < num_files; ++file_num) {
        // Create subdirectories
        std::string subdir = base_dir + "/dir_" + std::to_string(file_num / 100);
        fs::create_directories(subdir);
        
        std::string filepath = subdir + "/file_" + std::to_string(file_num) + ".txt";
        std::ofstream file(filepath);
        
        if (!file.is_open()) {
            std::cerr << "Failed to create: " << filepath << std::endl;
            continue;
        }
        
        // 20% of files will contain the keyword
        bool has_keyword = (dist(gen) % 100) < 20;
        
        for (int line_num = 0; line_num < lines_per_file; ++line_num) {
            bool include_in_line = has_keyword && (dist(gen) % lines_per_file) < 3;
            file << generate_line(gen, dist, include_in_line) << std::endl;
        }
        
        file.close();
        
        if ((file_num + 1) % 500 == 0) {
            std::cout << "  Created " << (file_num + 1) << " files..." << std::endl;
        }
    }
    
    std::cout << "✓ Test data generation complete!" << std::endl;
    std::cout << "  Directory: " << base_dir << std::endl;
    std::cout << "  Total files: " << num_files << std::endl;
    std::cout << "  Lines per file: " << lines_per_file << std::endl;
}

int main(int argc, char* argv[]) {
    int num_files = 2000;        // Number of files
    int lines_per_file = 500;    // Lines per file
    
    if (argc > 1) {
        num_files = std::stoi(argv[1]);
    }
    if (argc > 2) {
        lines_per_file = std::stoi(argv[2]);
    }
    
    std::string test_dir = "large_test_data";
    
    // Clean up old test data
    if (fs::exists(test_dir)) {
        std::cout << "Removing old test data..." << std::endl;
        fs::remove_all(test_dir);
    }
    
    create_test_files(test_dir, num_files, lines_per_file);
    
    return 0;
}