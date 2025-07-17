#pragma once
#ifndef COMPILATIONENGINE_HPP
#define COMPILATIONENGINE_HPP

#include "JackTokenizer.hpp"
#include "SymbolTable.hpp"
#include "VMWriter.hpp"

class CompilationEngine {
private:
    JackTokenizer& tokenizer;
    VMWriter& vmWriter;
    SymbolTable symbolTable;
    std::string currentClassName;
    int labelCount;

public:
    CompilationEngine(JackTokenizer &tokenizer, VMWriter &vmWriter);

    void compileClass();
    void compileClassVarDec();
    void compileSubroutine();
    void compileParameterList();
    void compileSubroutineBody();
    void compileVarDec();
    void compileStatements();
    void compileLet();
    void compileIf();
    void compileWhile();
    void compileDo();
    void compileReturn();
    void compileExpression();
    void compileTerm();
    void compileSubroutineCall();

    bool isOperator(const std::string& word);
    std::string currentTokenValue();
    std::string currentTokenType();
    VMWriter::Segment kindToSegment(SymbolTable::symbolKind kind);

};

#endif
