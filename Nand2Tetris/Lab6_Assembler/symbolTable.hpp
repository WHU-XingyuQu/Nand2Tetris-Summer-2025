#ifndef SYMBOLTABLE_HPP
#define SYMBOLTABLE_HPP

#include <unordered_map>
#include <string>
#include <memory>
#include <iostream>

class SymbolTable {
public:
    SymbolTable(){
        // Predefined symbol
        _symbolTable["SP"] = 0;
        _symbolTable["LCL"] = 1;
        _symbolTable["ARG"] = 2;
        _symbolTable["THIS"] = 3;
        _symbolTable["THAT"] = 4;
        for(int i=0;i<16;i++){
            _symbolTable["R"+std::to_string(i)] = i;
        }
        _symbolTable["SCREEN"] = 16384;
        _symbolTable["KBD"] = 24576;
    }

    void addEntry(const std::string& symbol, int address) {
        _symbolTable[symbol] = address;
    }

    bool contains(const std::string& symbol) const {
        return _symbolTable.find(symbol) != _symbolTable.end();
    }

    int getAddress(const std::string& symbol) const {
        auto it = _symbolTable.find(symbol);
        if (it != _symbolTable.end()) {
            return it->second;
        }
        throw std::runtime_error("Symbol not found: " + symbol);
    }

    void printSymbolTable() const {
        for (const auto& pair : _symbolTable) {
            std::cout << pair.first << " = " << pair.second << std::endl;
        }
    }

private:
    std::unordered_map<std::string,int> _symbolTable;
};

#endif // SYMBOLTABLE_HPP