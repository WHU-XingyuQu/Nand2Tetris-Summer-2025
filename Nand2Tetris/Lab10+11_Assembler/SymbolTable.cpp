#include "SymbolTable.hpp"

SymbolTable::SymbolTable():staticCount(0),fieldCount(0),argCount(0),varCount(0){ }

SymbolTable::~SymbolTable(){
    std::cout<<"SymbolTable desturcted."<<"\n";
}

void SymbolTable::reset(){
    //在调用函数时将子程序作用域的符号表清空
    subroutineScope.clear();
    argCount=0;
    varCount=0;
}

void SymbolTable::addEntry(const std::string &name,const std::string &type, symbolKind kind){
    // 官方指导文档中的define,为了避免宏定义指令"define"的歧义，改名为addEntry
    int index=0;
    switch(kind){
        case STATIC:
            index=staticCount++;
            break;
        case FIELD:
            index=fieldCount++;
            break;
        case ARG:
            index=argCount++;
            break;
        case VAR:
            index=varCount++;
            break;
        default:
            throw std::runtime_error("Symbol type error when adding entry.");
            return ;
    }

    SymbolInfo info {type,kind,index};  

    if(kind == STATIC || kind == FIELD){
        classScope[name]=info;
    }
    else{
        subroutineScope[name]=info;
    }
    std::cout<<"[debug]"<<name<<' '<<type<<' '<<index<<'\n';
}

int SymbolTable::countOfVar(symbolKind kind){
    switch (kind)
    {
        case STATIC:
            return staticCount;
        case FIELD:
            return fieldCount;
        case ARG:
            return argCount;
        case VAR:
            return varCount;
        
        default:
            throw std::runtime_error("Symbol type errors when counting symbol.");
            return NONE;
    }
}

SymbolTable::symbolKind SymbolTable::kindOf(const std::string &name) const{
    auto it=subroutineScope.find(name);
    if(it!=subroutineScope.end()){
        return it->second.kind;
    }
    it=classScope.find(name);
    if(it!=classScope.end()){
        return it->second.kind;
    }
    return NONE;
}

std::string SymbolTable::typeOf(const std::string& name) const {
    auto it = subroutineScope.find(name);
    if (it != subroutineScope.end()) return it->second.type;
    it = classScope.find(name);
    if (it != classScope.end()) return it->second.type;
    return "";
}

int SymbolTable::indexOf(const std::string& name) const {
    auto it = subroutineScope.find(name);
    if (it != subroutineScope.end()) return it->second.index;
    it = classScope.find(name);
    if (it != classScope.end()) return it->second.index;
    return -1;
}

std::string SymbolTable::kindToString(symbolKind kind){
     switch (kind)
    {
        case STATIC:
            return "static";
        case FIELD:
            return "field";
        case ARG:
            return "argument";
        case VAR:
            return "var";
        
        default:
            throw std::runtime_error("Symbol type errors when to_string symbol's kind.");
    }
}