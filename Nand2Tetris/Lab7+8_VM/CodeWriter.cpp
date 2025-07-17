#include "CodeWriter.hpp"

CodeWriter::CodeWriter(const std::string &filePath) {
    outputFile.open(filePath, std::ios::trunc);
    segment["argument"] = "ARG";
    segment["local"]    = "LCL";
    segment["static"]   = "16";
    segment["this"]     = "THIS";
    segment["that"]     = "THAT";
    segment["pointer"]  = "R3";
    segment["temp"]     = "R5";
    operators["add"] = {"+", "Binary"};
    operators["sub"] = {"-", "Binary"};
    operators["neg"] = {"-", "Unary"};
    operators["eq"]  = {"JEQ", "Logic"};
    operators["gt"]  = {"JGT", "Logic"};
    operators["lt"]  = {"JLT", "Logic"};
    operators["and"] = {"&", "Binary"};
    operators["or"]  = {"|", "Binary"};
    operators["not"] = {"!", "Unary"};
    callCounter.clear();
    currentFunction.clear();
}

CodeWriter::~CodeWriter() {
    if (outputFile.is_open()) outputFile.close();
}

void CodeWriter::setFileName(const std::string &filePath) {
    auto idx1 = filePath.find_last_of("/\\");
    auto idx2 = filePath.find_last_of('.');
    outFile = filePath.substr(idx1 + 1, idx2 - idx1 - 1);
}

void CodeWriter::writeInit() {
    outputFile << "@256\n"
               << "D=A\n"
               << "@SP\n"
               << "M=D\n";
    // call Sys.init 0
    writeCall("Sys.init", 0);
}

void CodeWriter::writeArithmetic(const std::string &cmd){
    std::string output;
    if(operators[cmd].second=="Binary"){
        output=translate.BinaryOperation(operators[cmd].first);
    }
    else if(operators[cmd].second=="Unary"){
        output=translate.UnaryOperation(operators[cmd].first);
    }
    else if(operators[cmd].second=="Logic")
        output = translate.LogicOperation(operators[cmd].first, outFile + "_" + std::to_string(index++));
    else{
        throw std::runtime_error("Invalid Ar.command.");
    }
    outputFile<<output;
}

void CodeWriter::writePushPop(Parser::Command &cmd, const std::string &seg, int16_t index){
    if(cmd.type!=Parser::Command::C_PUSH&&
        cmd.type!=Parser::Command::C_POP){
            throw std::runtime_error("Invalid PP.command.");
        }
    std::string output;
    if(cmd.command=="push"){
        if(cmd.arg1=="constant"){
            output=translate.PushConstant(cmd.arg2);
        }
        else if(cmd.arg1=="pointer"||cmd.arg1=="temp"){
            output="@"+segment[cmd.arg1]+"\n"
                    "D=M\n"
                    "@"+std::to_string(cmd.arg2)+"\n"
                    "A=D+A\nD=M\n"
                    +translate.LoadDtoSPPos()
                    +translate.SPAdd();
        }
        else if(cmd.arg1=="static"){
            output="@"+outFile+"."+std::to_string(cmd.arg2)+"\n"
                    "D=M\n"
                    "@SP\n"
                    "A=M\n"
                    "M=D\n"+translate.SPAdd();
        }
        else{
            output=translate.LoadSegValToDR(segment[cmd.arg1],cmd.arg2)+
                    translate.LoadDtoSPPos()+
                    translate.SPAdd();
        }
    }
    else if(cmd.command=="pop"){
        std::string str;
        if (cmd.arg1 == "temp" || cmd.arg1 == "pointer") {
            output = translate.SPSub() +
                    translate.LoadStackTopValToDR() +
                    "@R13\n"
                    "M=D\n" +  // 暂存值
                    "@" + segment[cmd.arg1] + "\n"
                    "D=M\n" +//change
                    "@" + std::to_string(cmd.arg2) + "\n"
                    "D=D+A\n" +
                    "@R14\n"
                    "M=D\n" +  // 目标地址
                    "@R13\n"
                    "D=M\n" +  // 取回值
                    "@R14\n"
                    "A=M\n"
                    "M=D\n";
        }
        else if(cmd.arg1=="static"){
            output=translate.SPSub()+
                    "A=M\n"+
                    "D=M\n"+
                    "@"+outFile+"."+std::to_string(cmd.arg2)+"\n"
                    "M=D\n";
        }
        else{
            output="@"+segment[cmd.arg1]+"\n"+
            "D=M\n"
            "@"+std::to_string(cmd.arg2)+"\n"
            "D=D+A\n"
            "@R13\n"
            "M=D\n"+translate.SPSub()+
            translate.LoadStackTopValToDR()+
            "@R13\n"
            "A=M\n"
            "M=D\n";
        }
    }
    outputFile<<output;
}


void CodeWriter::writeFunction(const std::string &functionName, int16_t numLocals) {
    currentFunction = functionName;
    // (functionName)
    outputFile << "(" << functionName << ")\n";
    // 初始化局部变量为0
    for (int i = 0; i < numLocals; ++i) {
        outputFile << "@0\n"
                   << "D=A\n"
                   << "@SP\n"
                   << "M=M+1\n"
                   << "A=M-1\n"
                   << "M=D\n";
    }
}

void CodeWriter::writeCall(const std::string &functionName, int16_t numArgs) {
    // 唯一的返回地址标签
    std::string retLabel = functionName + "$ret." + std::to_string(callCounter[functionName]++);
    // 1) push return-address
    outputFile << "@" << retLabel << "\n"
               << "D=A\n"
               << "@SP\nM=M+1\n"
               << "A=M-1\nM=D\n";
    // 2) push LCL
    outputFile << "@LCL\n"
               << "D=M\n"
               << "@SP\nM=M+1\n"
               << "A=M-1\nM=D\n";
    // 3) push ARG
    outputFile << "@ARG\n"
               << "D=M\n"
               << "@SP\nM=M+1\n"
               << "A=M-1\nM=D\n";
    // 4) push THIS
    outputFile << "@THIS\n"
               << "D=M\n"
               << "@SP\nM=M+1\n"
               << "A=M-1\nM=D\n";
    // 5) push THAT
    outputFile << "@THAT\n"
               << "D=M\n"
               << "@SP\nM=M+1\n"
               << "A=M-1\nM=D\n";
    // ARG = SP - 5 - numArgs
    outputFile << "@SP\nD=M\n"
               << "@5\nD=D-A\n"
               << "@" << numArgs << "\nD=D-A\n"
               << "@ARG\nM=D\n";
    // LCL = SP
    outputFile << "@SP\nD=M\n"
               << "@LCL\nM=D\n";
    // goto functionName
    outputFile << "@" << functionName << "\n0;JMP\n";
    // (retLabel)
    outputFile << "(" << retLabel << ")\n";
}

void CodeWriter::writeReturn() {
    // FRAME = LCL
    outputFile << "@LCL\nD=M\n@R13\nM=D\n";
    // RET = *(FRAME-5)
    outputFile << "@5\nA=D-A\nD=M\n@R14\nM=D\n";
    // *ARG = pop()
    outputFile << "@SP\nAM=M-1\nD=M\n@ARG\nA=M\nM=D\n";
    // SP = ARG+1
    outputFile << "@ARG\nD=M+1\n@SP\nM=D\n";
    // THAT = *(FRAME-1)
    outputFile << "@R13\nAM=M-1\nD=M\n@THAT\nM=D\n";
    // THIS = *(FRAME-2)
    outputFile << "@R13\nAM=M-1\nD=M\n@THIS\nM=D\n";
    // ARG = *(FRAME-3)
    outputFile << "@R13\nAM=M-1\nD=M\n@ARG\nM=D\n";
    // LCL = *(FRAME-4)
    outputFile << "@R13\nAM=M-1\nD=M\n@LCL\nM=D\n";
    // goto RET
    outputFile << "@R14\nA=M\n0;JMP\n";
}

void CodeWriter::writeLabel(const std::string &label) {
    std::string scoped = currentFunction.empty()
        ? label
        : currentFunction + "$" + label;
    outputFile << "(" << scoped << ")\n";
}

void CodeWriter::writeGoto(const std::string &label) {
    std::string scoped = currentFunction.empty()
        ? label
        : currentFunction + "$" + label;
    outputFile << "@" << scoped << "\n0;JMP\n";
}

void CodeWriter::writeIf(const std::string &label) {
    std::string scoped = currentFunction.empty()
        ? label
        : currentFunction + "$" + label;
    outputFile << "@SP\nAM=M-1\nD=M\n"
               << "@" << scoped << "\nD;JNE\n";
}
