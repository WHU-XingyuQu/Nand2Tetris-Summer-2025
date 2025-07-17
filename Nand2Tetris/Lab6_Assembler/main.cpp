#include "code.hpp"
#include "parser.hpp"
#include "symbolTable.hpp"

#include <iostream>
#include <vector>
#include <bitset>
#include <string>
#include <unordered_map>
#include <fstream>
#include <algorithm>
#include <memory>

std::vector<std::string> filename={
    "Add.asm","Add.hack",
    "Max.asm","Max.hack",
    "MaxL.asm","MaxL.hack",
    "Pong.asm","Pong.hack",
    "PongL.asm","PongL.hack",
    "Rect.asm","Rect.hack",
    "RectL.asm","RectL.hack",
};

int main() {
    for(size_t j=0;j<filename.size();j+=2){
        Parser parser(filename[j], filename[j+1]);
        for(size_t i=0;i<parser.allLines.size();i++){
            parser.advance();
        }
    }
    return 0;
}