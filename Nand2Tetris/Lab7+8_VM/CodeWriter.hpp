#pragma once
#ifndef CODEWRITER_HPP
#define CODEWRITER_HPP

#include "parser.hpp"

class CodeWriter
{
public:
    CodeWriter(const std::string &outputFileName);
    ~CodeWriter();

    void setFileName(const std::string &filePath);

    struct Translate
    {
        inline std::string SPAdd()
        {
            return "@SP\nM=M+1\n";
            // 入栈：在实际计算中，后一个“出栈”的计算数实际上还在原来的内存存放位置，所以可以直接写入这块内存，相当于把计算结果“入栈”
        }

        inline std::string SPSub()
        {
            return "@SP\nM=M-1\n"; // 出栈
        }

        inline std::string LoadValToDR(uint16_t constantVal)
        {
            return "@" + std::to_string(constantVal) +
                   "\nD=A\n";
        }

        inline std::string LoadStackTopValToDR()
        {
            return "A=M\nD=M\n";
        }

        inline std::string LoadSegValToDR(std::string Seg, uint16_t offset)
        {
            return "@" + Seg + "\nD=M\n" + "@" + std::to_string(offset) + "\nA=D+A\nD=M\n";//change
        }

        inline std::string LoadDtoSPPos()
        {
            return "@SP\nA=M\nM=D\n";
        }

        inline std::string BinaryOperation(const std::string &op) {
               // 规范形式：D 是弹出的第一个值 (y)，M 是第二个值 (x)
            // 对 add、and、or：x = D + M  => M=D+M
            if (op == "+") {
                return
                "@SP\nAM=M-1\n"   // SP--, A=SP
                    "D=M\n"           // D=y
                    "@SP\nAM=M-1\n"   // SP--, A=SP
                    "M=D+M\n"         // x = x + y
                    "@SP\nM=M+1\n";   // SP++
                }
                // 对 sub：x = x - y => M=M-D 但写成 D=M-D
                else if (op == "-") {
                return
                    "@SP\nAM=M-1\n"
                    "D=M\n"
                    "@SP\nAM=M-1\n"
                    "M=M-D\n"         // M = x - y
                    "@SP\nM=M+1\n";
                }
                // 对 and/or 类似 add 的写法
                else {
                return
                    "@SP\nAM=M-1\n"
                    "D=M\n"
                    "@SP\nAM=M-1\n"
                    "M=D" + op + "M\n" // D&M 或 D|M
                    "@SP\nM=M+1\n";
                }
        }


        inline std::string LogicOperation(const std::string &op, std::string idx)
        {
            return SPSub() +"A=M\nD=M\n" 
                    +SPSub() +"A=M\nD=M-D\n"
                   "@TRUE" +
                   idx + "\nD;"+ op + "\n" + // JGT JLT JEQ
                   "@SP\nA=M\n"
                   "M=0\n"
                   "@END" +
                   idx + "\n"
                        "0;JMP\n"
                        "(TRUE" +
                   idx + ")\n"
                        "@SP\nA=M\n"
                        "M=-1\n"
                        "(END" +
                   idx + ")\n" +
                   SPAdd();
        }

        inline std::string UnaryOperation(const std::string &op)
        {
            return SPSub() + "A=M\n" + "M=" + op + "M\n" + SPAdd();//only neg
        }

        inline std::string LoadSymbolToAR(const std::string &sym)
        {
            return "@" + sym + "\n";
        }

        inline std::string GenerateSymbol(const std::string &sym)
        {
            return "(" + sym + ")\n";
        }

        inline std::string GenerateGoToFunction(const std::string &func)
        {
            return LoadSymbolToAR(func)+
                    "0;JMP\n";
        }

        inline std::string PushSymbol(const std::string &sym)
        {
            return LoadSymbolToAR(sym) + "D=A\n" +
                   LoadDtoSPPos() + SPAdd();
        }

        inline std::string PushRegVal(const std::string reg)
        {
            return LoadSymbolToAR(reg) + "D=M\n" +
                   LoadDtoSPPos() + SPAdd();
        }

        inline std::string PushConstant(uint16_t val)
        {
            return "@" + std::to_string(val) + "\n"
                    "D=A\n"
                    "@SP\n"
                    "M=M+1\n"
                    "A=M-1\n"
                    "M=D\n";
        }
    };

    void writeArithmetic(const std::string &command);
    void writePushPop(Parser::Command &command, const std::string &seg, int16_t index);
    void writeLabel(const std::string &label);
    void writeGoto(const std::string &label);
    void writeIf(const std::string &label);
    void writeFunction(const std::string &functionName, int16_t numLocals);
    void writeReturn();
    void writeCall(const std::string &functionName, int16_t numArgs);
    void writeInit();

    std::string outFile;
    std::ofstream outputFile;
    std::unordered_map<std::string, std::string> segment;
    std::unordered_map<std::string, std::pair<std::string, std::string>> operators;
    std::unordered_map<std::string, uint16_t> returnAddress;
    std::unordered_map<std::string,int> callCounter;
    int index;

    std::string currentFunction;

    Translate translate;
};

#endif