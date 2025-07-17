#ifndef SYMBOLTABLE_HPP
#define SYMBOLTABLE_HPP

#include <iostream>
#include <string>
#include <unordered_map>

// SymbolTable 类负责管理 Jack 语言变量的符号表
// 分为 class 级和 subroutine 级两张表，生命周期独立
// 提供查询变量的类型(kind/type/index)的接口

class SymbolTable{
public:
    enum symbolKind{
        STATIC,
        FIELD,
        ARG,
        VAR,
        NONE
    };

    struct SymbolInfo{
        std::string type; //Jack定义的变量类型
        symbolKind kind;  //symbolKind
        int index;        //序号
    };
    

public:
    SymbolTable();
    ~SymbolTable();
    void reset();
    void addEntry(const std::string &name,const std::string &type, symbolKind kind);//define
    int countOfVar(symbolKind kind);
    symbolKind kindOf(const std::string &name) const;    
    std::string typeOf(const std::string &name) const;
    int indexOf(const std::string &name) const;
    static std::string kindToString(symbolKind kind);

private:
    //两层符号表
    std::unordered_map<std::string,SymbolInfo> classScope;
    std::unordered_map<std::string,SymbolInfo> subroutineScope;

    //四种类型用四个计数器维护
    int staticCount;
    int fieldCount;
    int argCount;
    int varCount;

    
};

#endif