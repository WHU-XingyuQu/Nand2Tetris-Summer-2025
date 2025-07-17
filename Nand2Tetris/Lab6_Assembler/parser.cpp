#include "symbolTable.hpp"
#include "parser.hpp"
#include <algorithm>
#include <string>
#include <bitset>

    Parser::Parser(const std::string& infile, const std::string& outfile)
    : _inputFile(infile), _outputFile(outfile,std::ios::binary), _curLine(0), _curPos(16), _curIndex(0) {

        if (!_inputFile.is_open()) {
            throw std::runtime_error("Cannot open: " + infile);
        }
        replaceLabel(); // First pass to replace labels
    
    }

    Parser::~Parser() {
        if (_inputFile.is_open()) {
            _inputFile.close();
        }
    }

    void Parser::replaceLabel(){
        std::string line;
        std::size_t lineNumber = 0;

        // First pass: label parsing
        while (std::getline(_inputFile, line)) {
            auto code = removeSpaceAndComment(line);
            if (code.empty()) continue;
            if (code[0] == '(') {
                symbolTable.addEntry(code.substr(1, code.length() - 2), lineNumber);
            } 
            else {
                ++lineNumber;
                allLines.push_back(code);
            }
        }
        _inputFile.close();
    }

    bool Parser::hasMoreLines(){
        return _curIndex < allLines.size();
    }

    void Parser::advance() {
        if(!hasMoreLines()) return ;

        _currentLine = allLines[_curIndex++]; 

        if(instructionType()==0){
            ACommand(_currentLine);
        }
        else if(instructionType()==1){
            CCommand(_currentLine);
        }
    }

    int Parser::instructionType(){
        if (_currentLine.empty()) {
            throw std::runtime_error("No current line to analyze.");
        }
        if (_currentLine[0] == '@') {
            return 0; // A-instruction
        }
        else if(_currentLine[0]=='('){
            return 2; // L-instruction (label)
        }
        else {
            return 1; // C-instruction
        }
    }

    std::string Parser::currentLine() {
        return _currentLine;
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

    std::string Parser::DecimalToBinary(const std::string &val){
        std::string tmpStr = val;
        int n = std::stoi(tmpStr);
        std::string binary=std::bitset<15>(n).to_string();
        while (binary.length() < 15) {
            binary = "0" + binary;
        }
        return binary;
    }

    void Parser::ACommand(const std::string &str){
        auto symbol = str.substr(1);
        int address;
        if (isdigit(symbol[0])) { 
            address = std::stoi(symbol);
        } 
        else {
            if (!symbolTable.contains(symbol)) {
                symbolTable.addEntry(symbol, _curPos++);
            }
            address = symbolTable.getAddress(symbol);
        }

        _outputFile << "0" << DecimalToBinary(std::to_string(address)) << "\n";
        
    }


    void Parser::CCommand(const std::string &str){
        std::string destPart, compPart, jumpPart;
        size_t eqPos = str.find('=');
        size_t semiPos = str.find(';');

        if(eqPos!=-1){
            destPart = str.substr(0, eqPos); // Get the part before '='
        } 
        else {
            destPart = "null"; // No dest part
            eqPos=0;
        }

        if(semiPos!=-1){
            jumpPart = str.substr(semiPos + 1); // Get the part after ';'
        }
        else {
            jumpPart = "null"; // No jump part
            semiPos=str.length();
        }

        std::size_t compPos;

        if(eqPos==0){
            compPos = 0; // If there is no dest part, comp starts from the beginning
        } else {
            compPos = eqPos + 1; // If there is a dest part, comp starts after '='
        }

        compPart = str.substr(compPos, semiPos - compPos); // Get the part between '=' and ';'

        _outputFile << "111" << code.comp(compPart) << code.dest(destPart) << code.jump(jumpPart) << '\n'; // Write C-instruction in binary format
    }
