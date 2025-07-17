#include "CompilationEngine.hpp"

CompilationEngine::CompilationEngine(JackTokenizer &tokenizer, VMWriter &vmWriter)
: tokenizer(tokenizer), vmWriter(vmWriter),labelCount(0) {
    tokenizer.token_index = 0;
}

void CompilationEngine::compileClass() {
    tokenizer.token_index++; // 'class'
    currentClassName = tokenizer.tokens[tokenizer.token_index++].first;
    tokenizer.token_index++; // '{'

    while (currentTokenValue() == "static" || currentTokenValue() == "field") {
        std::cout << "current token at class-level: " << currentTokenValue() << std::endl;

        compileClassVarDec();
    }

    while (currentTokenValue() == "constructor" || currentTokenValue() == "function" || currentTokenValue() == "method") {
        std::cout << "current token at class-level: " << currentTokenValue() << std::endl;

        compileSubroutine();
    }

    tokenizer.token_index++; // '}'
}

void CompilationEngine::compileClassVarDec() {
    std::string kindStr = tokenizer.tokens[tokenizer.token_index++].first;
    SymbolTable::symbolKind kind = (kindStr == "static") ? SymbolTable::STATIC : SymbolTable::FIELD;

    std::string type = tokenizer.tokens[tokenizer.token_index++].first;

    while (true) {
        std::string name = tokenizer.tokens[tokenizer.token_index++].first;
        symbolTable.addEntry(name, type, kind);

        if (currentTokenValue() == ",") {
            tokenizer.token_index++; // consume ','
        } else {
            break;
        }
    }

    tokenizer.token_index++; // consume ';'
}


void CompilationEngine::compileSubroutine() {
    symbolTable.reset();

    std::string subType = tokenizer.tokens[tokenizer.token_index++].first; // constructor/function/method
    tokenizer.token_index++; // return type
    std::string subName = tokenizer.tokens[tokenizer.token_index++].first;

    tokenizer.token_index++; // '('
    compileParameterList();
    tokenizer.token_index++; // ')'

    tokenizer.token_index++; // '{'

    while (currentTokenValue() == "var") {
        compileVarDec();
    }

    int nLocals = symbolTable.countOfVar(SymbolTable::VAR);

    vmWriter.writeFunction(currentClassName + "." + subName, nLocals);

    compileStatements();

    tokenizer.token_index++; // '}'
}


void CompilationEngine::compileParameterList() {
    while (currentTokenValue() != ")") {
        std::string type = tokenizer.tokens[tokenizer.token_index++].first;
        std::string name = tokenizer.tokens[tokenizer.token_index++].first;
        symbolTable.addEntry(name, type, SymbolTable::ARG);

        if (currentTokenValue() == ",") tokenizer.token_index++;
    }
}

void CompilationEngine::compileVarDec() {
    tokenizer.token_index++; // 'var'
    std::string type = tokenizer.tokens[tokenizer.token_index++].first;

    while (true) {
        std::string name = tokenizer.tokens[tokenizer.token_index++].first;
        symbolTable.addEntry(name, type, SymbolTable::VAR);

        if (currentTokenValue() == ",") {
            tokenizer.token_index++; // skip ','
        } else {
            break;
        }
    }

    tokenizer.token_index++; // skip ';'
}

void CompilationEngine::compileStatements() {
    while (true) {
        std::string kw = currentTokenValue();
        std::cout << "[debug] statement token: " << currentTokenValue() << "\n";
        if (kw == "let") compileLet();
        else if (kw == "if") compileIf();
        else if (kw == "while") compileWhile();
        else if (kw == "do") compileDo();
        else if (kw == "return") compileReturn();
        else break;
    }
}

void CompilationEngine::compileLet() {
    tokenizer.token_index++; // 'let'
    std::string varName = tokenizer.tokens[tokenizer.token_index++].first;
    std::cout << "[debug] let target: " << varName << "\n";


    bool isArray = false;

    if (currentTokenValue() == "[") {
        isArray = true;
        tokenizer.token_index++; // '['
        compileExpression();     // offset

        SymbolTable::symbolKind kind = symbolTable.kindOf(varName);
        int index = symbolTable.indexOf(varName);
        auto seg = kindToSegment(kind);

        vmWriter.writePush(seg, index);   // push base
        vmWriter.writeArithmetic(VMWriter::ADD);

        tokenizer.token_index++; // ']'
    }

    tokenizer.token_index++; // '='
    compileExpression();     // RHS

    if (isArray) {
        vmWriter.writePop(VMWriter::TEMP, 0);       // store RHS value
        vmWriter.writePop(VMWriter::POINTER, 1);   // THAT = base+offset
        vmWriter.writePush(VMWriter::TEMP, 0);     // reload value
        vmWriter.writePop(VMWriter::THAT, 0);      // *THAT = value
    } else {
        SymbolTable::symbolKind kind = symbolTable.kindOf(varName);
        int index = symbolTable.indexOf(varName);
        auto seg = kindToSegment(kind);
        vmWriter.writePop(seg, index);
    }

    tokenizer.token_index++; // ';'
}


void CompilationEngine::compileIf() {
    tokenizer.token_index++; // 'if'
    tokenizer.token_index++; // '('
    compileExpression();
    tokenizer.token_index++; // ')'

    std::string labelElse = currentClassName + "_IF_ELSE_" + std::to_string(labelCount++);
    std::string labelEnd  = currentClassName + "_IF_END_"  + std::to_string(labelCount++);

    vmWriter.writeArithmetic(VMWriter::NOT);
    vmWriter.writeIf(labelElse);

    tokenizer.token_index++; // '{'
    compileStatements();
    tokenizer.token_index++; // '}'

    vmWriter.writeGoto(labelEnd);
    vmWriter.writeLabel(labelElse);

    if (currentTokenValue() == "else") {
        tokenizer.token_index++; // 'else'
        tokenizer.token_index++; // '{'
        compileStatements();
        tokenizer.token_index++; // '}'
    }

    vmWriter.writeLabel(labelEnd);
}


void CompilationEngine::compileWhile() {
    tokenizer.token_index++; // 'while'

    std::string labelStart = currentClassName + "_WHILE_START_" + std::to_string(labelCount++);
    std::string labelEnd   = currentClassName + "_WHILE_END_"   + std::to_string(labelCount++);

    vmWriter.writeLabel(labelStart);

    tokenizer.token_index++; // '('
    compileExpression();
    tokenizer.token_index++; // ')'

    vmWriter.writeArithmetic(VMWriter::NOT);
    vmWriter.writeIf(labelEnd);

    tokenizer.token_index++; // '{'
    compileStatements();
    tokenizer.token_index++; // '}'

    vmWriter.writeGoto(labelStart);
    vmWriter.writeLabel(labelEnd);
}


void CompilationEngine::compileDo() {
    tokenizer.token_index++; // 'do'
    compileSubroutineCall();

    vmWriter.writePop(VMWriter::TEMP, 0); // discard return value
    tokenizer.token_index++; // ';'
}


void CompilationEngine::compileReturn() {
    tokenizer.token_index++; // 'return'
    if (currentTokenValue() != ";") {
        compileExpression();
    } else {
        vmWriter.writePush(VMWriter::CONST, 0); // void return
    }
    vmWriter.writeReturn();
    tokenizer.token_index++; // ';'
}

void CompilationEngine::compileExpression() {
    compileTerm();
    while (isOperator(currentTokenValue())) {
        std::string op = tokenizer.tokens[tokenizer.token_index++].first;
        compileTerm();

        if (op == "+") vmWriter.writeArithmetic(VMWriter::ADD);
        else if (op == "-") vmWriter.writeArithmetic(VMWriter::SUB);
        else if (op == "&") vmWriter.writeArithmetic(VMWriter::AND);
        else if (op == "|") vmWriter.writeArithmetic(VMWriter::OR);
        else if (op == "=") vmWriter.writeArithmetic(VMWriter::EQ);
        else if (op == "<") vmWriter.writeArithmetic(VMWriter::LT);
        else if (op == ">") vmWriter.writeArithmetic(VMWriter::GT);
    }
}

void CompilationEngine::compileTerm() {
    std::string token = tokenizer.tokens[tokenizer.token_index++].first;
    std::string type = tokenizer.tokens[tokenizer.token_index - 1].second;

    // 一元运算符
    if (token == "-" || token == "~") {
        compileTerm();
        if (token == "-")
            vmWriter.writeArithmetic(VMWriter::NEG);
        else
            vmWriter.writeArithmetic(VMWriter::NOT);
        return;
    }

    if (type == "integerConstant") {
        vmWriter.writePush(VMWriter::CONST, std::stoi(token));
    } 
    else if (type == "stringConstant") {
        vmWriter.writePush(VMWriter::CONST, token.size());
        vmWriter.writeCall("String.new", 1);
        for (char c : token) {
            vmWriter.writePush(VMWriter::CONST, static_cast<int>(c));
            vmWriter.writeCall("String.appendChar", 2);
        }
    }
    else if (token == "(") {
        compileExpression();
        tokenizer.token_index++; // ')'
    } 
    else if (token == "true") {
        vmWriter.writePush(VMWriter::CONST, 0);
        vmWriter.writeArithmetic(VMWriter::NOT);
    } 
    else if (token == "false" || token == "null") {
        vmWriter.writePush(VMWriter::CONST, 0);
    } 
    else if (token == "this") {
        vmWriter.writePush(VMWriter::POINTER, 0);
    } 
    else {
        std::string nextToken = currentTokenValue();

        if (nextToken == "[") {
            // 数组访问
            SymbolTable::symbolKind kind = symbolTable.kindOf(token);
            int index = symbolTable.indexOf(token);
            auto seg = kindToSegment(kind);

            vmWriter.writePush(seg, index);

            tokenizer.token_index++; // '['
            compileExpression();
            tokenizer.token_index++; // ']'

            vmWriter.writeArithmetic(VMWriter::ADD);
            vmWriter.writePop(VMWriter::POINTER, 1);
            vmWriter.writePush(VMWriter::THAT, 0);
        }
        else if (nextToken == "(" || nextToken == ".") {
            tokenizer.token_index--; // 回退
            compileSubroutineCall();
        }
        else {
            // 普通变量
            SymbolTable::symbolKind kind = symbolTable.kindOf(token);
            if (kind == SymbolTable::NONE) {
                throw std::runtime_error("Undefined variable: " + token);
            }
            int index = symbolTable.indexOf(token);
            auto seg = kindToSegment(kind);
            vmWriter.writePush(seg, index);
        }
    }
}

void CompilationEngine::compileSubroutineCall() {
    std::string name = tokenizer.tokens[tokenizer.token_index++].first;
    int nArgs = 0;

    if (currentTokenValue() == ".") {
        tokenizer.token_index++; // '.'
        std::string subroutine = tokenizer.tokens[tokenizer.token_index++].first;

        SymbolTable::symbolKind kind = symbolTable.kindOf(name);
        if (kind != SymbolTable::NONE) {
            // 对象方法
            auto seg = kindToSegment(kind);
            int index = symbolTable.indexOf(name);
            vmWriter.writePush(seg, index); // push object
            name = symbolTable.typeOf(name) + "." + subroutine;
            nArgs = 1;
        } else {
            // 类名.方法
            name = name + "." + subroutine;
        }
    } else {
        // 当前类的方法
        vmWriter.writePush(VMWriter::POINTER, 0); // push this
        name = currentClassName + "." + name;
        nArgs = 1;
    }

    tokenizer.token_index++; // '('
    if (currentTokenValue() != ")") {
        compileExpression();
        nArgs++;
        while (currentTokenValue() == ",") {
            tokenizer.token_index++; // ','
            compileExpression();
            nArgs++;
        }
    }
    tokenizer.token_index++; // ')'

    vmWriter.writeCall(name, nArgs);
}


std::string CompilationEngine::currentTokenValue() {
    if (tokenizer.token_index >= tokenizer.tokens.size()) return "";
    return tokenizer.tokens[tokenizer.token_index].first;
}

bool CompilationEngine::isOperator(const std::string &token) {
    return token == "+" || token == "-" || token == "*" || token == "/" ||
           token == "&" || token == "|" || token == "<" || token == ">" || token == "=";
}

VMWriter::Segment CompilationEngine::kindToSegment(SymbolTable::symbolKind kind) {
    switch(kind) {
        case SymbolTable::STATIC: return VMWriter::STATIC;
        case SymbolTable::FIELD:  return VMWriter::THIS;
        case SymbolTable::ARG:    return VMWriter::ARG;
        case SymbolTable::VAR:    return VMWriter::LOCAL;
        default: 
            std::cout<<"[debug]"<<kind<<"\n";
            throw std::runtime_error("Invalid.");
    }
}
