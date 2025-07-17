#include "Parser.hpp"

#define returnFalse 0xffff;

Parser::Parser(const std::string &srcPath):
    inputSrc(srcPath), srcFileName(srcPath) {
    if (!inputSrc.is_open()) {
        throw std::runtime_error("Failed to open source file: " + srcPath);
    }
}

Parser::~Parser() {
    if (inputSrc.is_open()) {
        inputSrc.close();
    }
}

bool Parser::hasMoreCommands() {
    return inputSrc.peek() != EOF;
}

void Parser::advance() {
    std::string line;
    while (std::getline(inputSrc, line)) {
        line = removeComment(line);


        if (line.empty()) {
            currentCommand.command = "";
            continue;
        }

        std::istringstream iss(line);
        std::string token_1, token_2, token_3;
        iss >> token_1 >> token_2 >> token_3;

        // 清除前后空格
        auto trim = [](const std::string& s) {
            size_t start = s.find_first_not_of(" \t\r\n");
            size_t end = s.find_last_not_of(" \t\r\n");
            if (start == std::string::npos) return std::string("");
            return s.substr(start, end - start + 1);
        };
        token_1 = trim(token_1);
        token_2 = trim(token_2);
        token_3 = trim(token_3);

        currentCommand.command = token_1;

        if (token_1 == "push") {
            currentCommand.type = Command::C_PUSH;
            currentCommand.arg1 = token_2;
            currentCommand.arg2 = std::stoi(token_3);
        } else if (token_1 == "pop") {
            currentCommand.type = Command::C_POP;
            currentCommand.arg1 = token_2;
            currentCommand.arg2 = std::stoi(token_3);
        } else if (token_1 == "label") {
            currentCommand.type = Command::C_LABEL;
            currentCommand.arg1 = token_2;
        } else if (token_1 == "goto") {
            currentCommand.type = Command::C_GOTO;
            currentCommand.arg1 = token_2;
        } else if (token_1 == "if-goto") {
            currentCommand.type = Command::C_IF;
            currentCommand.arg1 = token_2;
        } else if (token_1 == "function") {
            currentCommand.type = Command::C_FUNCTION;
            currentCommand.arg1 = token_2;
            currentCommand.arg2 = std::stoi(token_3);
        } else if (token_1 == "call") {
            currentCommand.type = Command::C_CALL;
            currentCommand.arg1 = token_2;
            currentCommand.arg2 = std::stoi(token_3);
        } else if (token_1 == "return") {
            currentCommand.type = Command::C_RETURN;
        } else {
            // 认为是算术逻辑命令
            currentCommand.type = Command::C_ARITHMETIC;
            currentCommand.arg1 = token_1;
        }

        return; // 读取到有效指令后退出
    }

    currentCommand.command = "";
}


Parser::Command::CommandType Parser::commandType() {
    return currentCommand.type;
}


std::string Parser::arg1(){
    if(currentCommand.type==Command::CommandType::C_RETURN){
        return "";
    }
    else if(currentCommand.type==Command::CommandType::C_ARITHMETIC){
        return currentCommand.command;
    }
    return currentCommand.arg1;
}

int16_t Parser::arg2(){
    if(currentCommand.type==Command::CommandType::C_PUSH||
        currentCommand.type==Command::CommandType::C_POP||
        currentCommand.type==Command::CommandType::C_FUNCTION||
        currentCommand.type==Command::CommandType::C_CALL){
            return currentCommand.arg2;
        }
    throw std::runtime_error("No arg2 exists.");
}

std::string Parser::removeSpaceAndComment(const std::string &str){
    return removeSpace(removeComment(str));
}

std::string Parser::removeSpace(const std::string &str){
    auto res=str;
    auto it=remove_if(res.begin(),res.end(),::isspace);
    res.erase(it,res.end());
    return res;
}

std::string Parser::removeComment(const std::string &str){
    return str.substr(0,str.find("//"));
}  