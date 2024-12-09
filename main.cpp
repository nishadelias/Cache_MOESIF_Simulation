// main.cpp

#include "CacheSystem.hpp"
#include "test_cache.cpp"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <regex>

int main(int argc, char* argv[]) {

    // TestCache::runAllTests();

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <inputfile.txt>" << std::endl;
        return 1;
    }

    CacheSystem system;
    std::ifstream inFile(argv[1]);
    std::string line;
    
    // Regular expression for parsing input lines
    std::regex pattern("P(\\d+): (read|write) <(\\d+)>");
    
    while (std::getline(inFile, line)) {
        std::smatch matches;
        if (std::regex_match(line, matches, pattern)) {
            int coreId = std::stoi(matches[1]);
            std::string operation = matches[2];
            uint32_t tag = std::stoul(matches[3]);
            
            if (operation == "read") {
                system.handleRead(coreId - 1, tag);  // Convert to 0-based index
            } else {
                system.handleWrite(coreId - 1, tag);
            }
        }
    }

    // Print statistics
    std::cout << system.getCacheHits() << std::endl;
    std::cout << system.getCacheMisses() << std::endl;
    std::cout << system.getWritebacks() << std::endl;
    std::cout << system.getBroadcasts() << std::endl;
    std::cout << system.getCacheToCache();  // No endl for last line
    
    return 0;
}