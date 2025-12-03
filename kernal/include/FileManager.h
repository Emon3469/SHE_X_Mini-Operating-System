#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <sstream>
#include <fstream>
#include <iostream>
#include <algorithm>
#include <cstdint>

namespace FileManager {

    static uint32_t numberofLines = 0;
    static std::stringstream readFile(std::string filepath) {
        try {
            numberofLines = 0;
            std::stringstream out;
            std::ifstream infile(filepath);
            std::string line;
            while (std::getline(infile, line)) {
                std::replace(line.begin(), line.end(), ',', ' ');
                out << line << '\n';
                ++numberofLines;
            }
            return out;
        }
        catch (...) {
            std::cout << "Error reading file...!" << std::endl;
            std::stringstream empty;
            return empty;
        }
    }

    static bool writeFile(std::string filename, std::string content_) {
        try {
            std::ofstream outfile(filename);
            std::stringstream content(content_);
            outfile << content.str();
            return true;
        }
        catch (...) {
            std::cout << "Error writing file...!" << std::endl;
        }
        return false;
    }
};

#endif  