#pragma once
#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <algorithm>

class Parser {
public:
    struct Command {
        enum CommandType {
            C_ARITHMETIC,
            C_PUSH,
            C_POP,
            C_LABEL,
            C_GOTO,
            C_IF,
            C_FUNCTION,
            C_RETURN,
            C_CALL
        };
        std::string command;
        CommandType type;
        std::string arg1;
        int16_t arg2;
    };
    Parser(const std::string &srcPath);
    ~Parser();
    bool hasMoreCommands();
    void advance();
    Command::CommandType commandType();
    std::string arg1();
    int16_t arg2();

    std::string removeSpaceAndComment(const std::string &line);
    std::string removeComment(const std::string &line);
    std::string removeSpace(const std::string &line);

    Command currentCommand;
    std::ifstream inputSrc;
    std::string srcFileName;

    std::unordered_set<std::string> arithOps = {
    "add", "sub", "neg", "eq", "gt", "lt", "and", "or", "not"
    };
};  

#endif // PARSER_HPP