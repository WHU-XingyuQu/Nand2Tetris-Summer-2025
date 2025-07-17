#include "JackTokenizer.hpp"
#include "CompilationEngine.hpp"
#include "VMWriter.hpp"
#include <iostream>
#include <dirent.h>
#include <sys/stat.h>

bool isDirectory(const std::string& path) {
    struct stat info;
    if (stat(path.c_str(), &info) != 0) return false;
    return (info.st_mode & S_IFDIR);
}

void compileFile(const std::string& path) {
    std::string outPath = path.substr(0, path.find_last_of('.')) + ".vm";

    JackTokenizer tokenizer(path);
    VMWriter vmWriter(outPath);
    CompilationEngine engine(tokenizer, vmWriter);
    engine.compileClass();

    std::cout << "Compiled: " << outPath << "\n";
}

void compileDirectory(const std::string& dirPath) {
    DIR* dir = opendir(dirPath.c_str());
    if (!dir) {
        std::cerr << "Cannot open directory: " << dirPath << "\n";
        return;
    }

    struct dirent* entry;
    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name == "." || name == "..") continue;

        std::string fullPath = dirPath + "/" + name;

        if (isDirectory(fullPath)) {
            compileDirectory(fullPath); // 递归子目录
        } else if (name.size() > 5 && name.substr(name.size() - 5) == ".jack") {
            compileFile(fullPath);
        }
    }

    closedir(dir);
}

int main() {
    std::string currentDir = ".";

    DIR* dir = opendir(currentDir.c_str());
    if (!dir) {
        std::cerr << "Cannot open current directory.\n";
        return 1;
    }

    struct dirent* entry;
    bool foundSubdir = false;

    while ((entry = readdir(dir)) != nullptr) {
        std::string name = entry->d_name;
        if (name == "." || name == "..") continue;

        if (isDirectory(name)) {
            foundSubdir = true;
            compileDirectory(name);
        }
    }

    closedir(dir);

    if (!foundSubdir) {
        std::cerr << "No subdirectories found.\n";
    }

    return 0;
}
