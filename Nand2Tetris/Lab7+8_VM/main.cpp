#include <iostream>
#include <vector>
#include <string>
#include <dirent.h>
#include <sys/types.h>
#include <cstring>
#include "parser.hpp"
#include "CodeWriter.hpp"

// 判断文件名是否以 .vm 结尾
static bool hasVmExtension(const std::string &filename) {
    return filename.size() >= 3 && filename.substr(filename.size() - 3) == ".vm";
}

// 从完整路径中提取文件名（不含目录）
static std::string basename(const std::string &path) {
    auto pos = path.find_last_of("/\\");
    if (pos == std::string::npos) return path;
    return path.substr(pos + 1);
}

int main() {
    std::vector<std::string> folders = {
        "BasicLoop",
        "FibonacciElement",
        "FibonacciSeries",
        "NestedCall",
        "SimpleFunction",
        "StaticsTest"
    };

    for (const auto &folder : folders) {
        // 收集所有 .vm 文件
        std::vector<std::string> vmFiles;
        DIR *dir = opendir(folder.c_str());
        if (!dir) {
            std::cerr << "[Error] Cannot open folder: " << folder << std::endl;
            continue;
        }
        struct dirent *entry;
        while ((entry = readdir(dir)) != nullptr) {
            std::string fn = entry->d_name;
            if (hasVmExtension(fn)) {
                vmFiles.push_back(folder + "/" + fn);
            }
        }
        closedir(dir);
        if (vmFiles.empty()) {
            std::cerr << "[Warning] No .vm files in " << folder << std::endl;
            continue;
        }

        // 创建输出 .asm 文件
        std::string outputAsm = folder + "/" + folder + ".asm";
        CodeWriter writer(outputAsm);

        bool hasSys = false;
        for (auto &path : vmFiles) {
            if (basename(path) == "Sys.vm") {
                hasSys = true;
                break;
            }
        }
        if (hasSys) {
            writer.writeInit();
        }

        for (auto &path : vmFiles) {
            Parser parser(path);
            writer.setFileName(path);
            while (parser.hasMoreCommands()) {
                parser.advance();
                if (parser.currentCommand.command.empty()) continue;
                switch (parser.commandType()) {
                    case Parser::Command::C_ARITHMETIC:
                        writer.writeArithmetic(parser.arg1());
                        break;
                    case Parser::Command::C_PUSH:
                    case Parser::Command::C_POP:
                        writer.writePushPop(parser.currentCommand,
                                            parser.arg1(),
                                            parser.arg2());
                        break;
                    case Parser::Command::C_LABEL:
                        writer.writeLabel(parser.arg1());
                        break;
                    case Parser::Command::C_GOTO:
                        writer.writeGoto(parser.arg1());
                        break;
                    case Parser::Command::C_IF:
                        writer.writeIf(parser.arg1());
                        break;
                    case Parser::Command::C_FUNCTION:
                        writer.writeFunction(parser.arg1(), parser.arg2());
                        break;
                    case Parser::Command::C_CALL:
                        writer.writeCall(parser.arg1(), parser.arg2());
                        break;
                    case Parser::Command::C_RETURN:
                        writer.writeReturn();
                        break;
                }
            }
        }

        std::cout << "[Success] " << folder << " → " << outputAsm << std::endl;
    }

    return 0;
}
