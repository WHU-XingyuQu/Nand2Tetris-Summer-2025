#ifndef _CODE_HPP
#define _CODE_HPP

#include <unordered_map>
#include <string>

class Code {
public:
    std::unordered_map<std::string, std::string> destTable,compTable,jumpTable;
    Code() {
        destTable["null"] = "000";
        destTable["M"] = "001";
        destTable["D"] = "010";
        destTable["MD"] = "011";
        destTable["A"] = "100";
        destTable["AM"] = "101";
        destTable["AD"] = "110";
        destTable["AMD"] = "111";

        compTable["0"] = "0101010";
        compTable["1"] = "0111111";
        compTable["-1"] = "0111010";
        compTable["D"] = "0001100";
        compTable["A"] = "0110000";
        compTable["!D"]= "0001101";
        compTable["!A"]= "0110001";
        compTable["-D"] = "0001111";
        compTable["-A"] = "0110011";
        compTable["D+1"] = "0011111";
        compTable["A+1"] = "0110111";
        compTable["D-1"] = "0001110";
        compTable["A-1"] = "0110010";
        compTable["D+A"] = "0000010";
        compTable["D-A"] = "0010011";
        compTable["A-D"] = "0000111";
        compTable["D&A"] = "0000000";
        compTable["D|A"] = "0010101";
        compTable["1+D"]="0011111";
        compTable["1+A"]="0110111";
        compTable["A+D"]="0000010";
        compTable["A&D"]="0000000";
        compTable["A|D"]="0010101";
        compTable["M"] = "1110000";
        compTable["!M"] = "1110001";
        compTable["-M"] = "1110011";
        compTable["M+1"] = "1110111";
        compTable["M-1"] = "1110010";
        compTable["D+M"] = "1000010";
        compTable["D-M"] = "1010011";
        compTable["M-D"] = "1000111";
        compTable["D&M"] = "1000000";
        compTable["D|M"] = "1010101";
        compTable["1+M"] = "1110111";
        compTable["M+D"]= "1000010";
        compTable["M&D"] = "1000000";
        compTable["M|D"] = "1010101";

        jumpTable["null"] = "000";
        jumpTable["JGT"] = "001";
        jumpTable["JEQ"] = "010";
        jumpTable["JGE"] = "011";
        jumpTable["JLT"] = "100";
        jumpTable["JNE"] = "101";
        jumpTable["JLE"] = "110";
        jumpTable["JMP"] = "111";

    }

public:
    std::string dest(const std::string &mnemonic) const {
        auto it = destTable.find(mnemonic);
        if (it != destTable.end()) {
            return it->second;
        }
        return "000"; // Default to null if not found
    }
    std::string comp(const std::string &mnemonic) const {
        auto it = compTable.find(mnemonic);
        if (it != compTable.end()) {
            return it->second;
        }
        return "0000000"; // Default to 0 if not found
    }
    std::string jump(const std::string &mnemonic) const {
        auto it = jumpTable.find(mnemonic);
        if (it != jumpTable.end()) {
            return it->second;
        }
        return "000"; // Default to null if not found
    }
};

#endif // _CODE_HPP