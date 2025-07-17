#ifndef VMWRITER_HPP
#define VMWRITER_HPP

#include "SymbolTable.hpp"
#include <fstream>
#include <iostream>
#include <string>

class VMWriter{
private:
    std::ofstream OFVM;
public:
    enum Segment{
        CONST,
        ARG,
        LOCAL,
        STATIC,
        THIS,
        THAT,
        POINTER,
        TEMP
    };

    enum Command{
        ADD,
        SUB,
        NEG,
        EQ,
        GT,
        LT,
        AND,
        OR,
        NOT
    };

public:
    VMWriter(const std::string &fileName);
    ~VMWriter();
    //push-pop
    void writePush(Segment segment,int index);
    void writePop(Segment segment,int index);
    //arithmatic
    void writeArithmetic(Command command);
    //if-goto
    void writeLabel(const std::string &label);
    void writeGoto(const std::string &label);
    void writeIf(const std::string &label);
    //call-return
    void writeCall(const std::string& name, int nArgs);
    void writeReturn();
    //function
    void writeFunction(const std::string& name, int nLocals);
    //indented
    void writeIndented();

private:
    std::string SegmentToStr(Segment segment) const;
    std::string CommandToStr(Command command) const;
};

#endif