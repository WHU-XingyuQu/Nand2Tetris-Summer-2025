#include "VMWriter.hpp"

VMWriter::VMWriter(const std::string &fileName){
    OFVM.open(fileName);
    if(!OFVM.is_open()){
        throw std::runtime_error("Can't open"+fileName+".");
    }
}

VMWriter::~VMWriter(){
    if(OFVM.is_open()){
        OFVM.close();
    }
}


    //push-pop
void VMWriter::writePush(Segment segment,int index){
    writeIndented();
    OFVM<<"push "<<SegmentToStr(segment)<<" "<<index<<"\n";
}

void VMWriter::writePop(Segment segment,int index){
    writeIndented();
    OFVM << "pop " << SegmentToStr(segment) << " " << index << "\n";
}
    //arithmatic
void VMWriter::writeArithmetic(Command command){
    writeIndented();
    OFVM << CommandToStr(command) << "\n";
}
    //if-goto
void VMWriter::writeLabel(const std::string &label){
    OFVM <<"label "<<label<<"\n";
}
void VMWriter::writeGoto(const std::string &label){
    writeIndented();
    OFVM<<"goto "<<label<<"\n";
}
void VMWriter::writeIf(const std::string &label){
    writeIndented();
    OFVM<<"if-goto "<<label<<"\n";
}
    //call-return
void VMWriter::writeCall(const std::string& name, int nArgs){
    writeIndented();
    OFVM<<"call "<<name<<" "<<nArgs<<"\n";
}
void VMWriter::writeReturn(){
    writeIndented();
    OFVM<<"return\n";
}
    //function
void VMWriter::writeFunction(const std::string& name, int nLocals){
    OFVM<<"function "<<name<<" "<<nLocals<<"\n";
}

void VMWriter::writeIndented(){
    OFVM<<"    ";
}

std::string VMWriter::SegmentToStr(Segment segment) const{
    switch (segment) {
        case CONST:   return "constant";
        case ARG:     return "argument";
        case LOCAL:   return "local";
        case STATIC:  return "static";
        case THIS:    return "this";
        case THAT:    return "that";
        case POINTER: return "pointer";
        case TEMP:    return "temp";
        default: 
            throw std::runtime_error("Error when to_string segment.");
    }
}
std::string VMWriter::CommandToStr(Command command) const{
    switch (command) {
        case ADD: return "add";
        case SUB: return "sub";
        case NEG: return "neg";
        case EQ:  return "eq";
        case GT:  return "gt";
        case LT:  return "lt";
        case AND: return "and";
        case OR:  return "or";
        case NOT: return "not";
        default:
            throw std::runtime_error("Error when to_string command.");
    }
}

