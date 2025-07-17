#ifndef PARESER_HPP
#define PARESER_HPP

#include "symbolTable.hpp"
#include "code.hpp"
#include <string>
#include <fstream>
#include <vector>

class Parser {
public:
    Parser(const std::string& InfileName,const std::string& OutFileName);
    ~Parser();
    bool hasMoreLines();
    void advance();
    int instructionType();
    // std::string symbol();
    // std::string dest();
    // std::string comp();
    // std::string jump();
    std::string currentLine();
    std::string removeSpaceAndComment(const std::string &str);
    std::string removeSpace(const std::string &str);
    std::string removeComment(const std::string &str);
    void replaceLabel();
    std::string DecimalToBinary(const std::string &val);
    void ACommand(const std::string &str);
    void CCommand(const std::string &str);
    void LCommand(const std::string &str);
    std::vector<std::string> allLines; 
public:
    std::ifstream _inputFile;
    std::ofstream _outputFile;
    std::string _currentLine;
    size_t _curLine;
    size_t _curPos;
    size_t _curIndex;
    SymbolTable symbolTable;
    Code code;
};

#endif // PARESER_HPP