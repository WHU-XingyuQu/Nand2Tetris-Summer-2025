#include "JackTokenizer.hpp"
#include <fstream>
#include <sstream>
#include <cctype>
#include <algorithm>

JackTokenizer::JackTokenizer(const std::string& inputFile) {
    std::ifstream ifs(inputFile);
    if (!ifs) throw std::runtime_error("Cannot open file: " + inputFile);

    std::ostringstream oss;
    oss << ifs.rdbuf();
    source = oss.str();

    removeComments();
    tokenize();
}

void JackTokenizer::removeComments() {
    std::string cleaned;
    bool inString = false;

    for (size_t i = 0; i < source.size(); ++i) {
        if (!inString && source[i] == '/' && source[i + 1] == '/') {
            while (i < source.size() && source[i] != '\n') ++i;
        }
        else if (!inString && source[i] == '/' && source[i + 1] == '*') {
            i += 2;
            while (i < source.size() && !(source[i] == '*' && source[i + 1] == '/')) ++i;
            ++i;
        }
        else {
            if (source[i] == '"') inString = !inString;
            cleaned += source[i];
        }
    }
    source = cleaned;
}

void JackTokenizer::tokenize() {
    static const std::string symbols = "{}()[].,;+-*/&|<>=~";
    static const std::vector<std::string> keywords = {
        "class","constructor","function","method","field","static","var","int",
        "char","boolean","void","true","false","null","this","let","do","if","else","while","return"
    };

    size_t pos = 0;

    while (pos < source.size()) {
        if (std::isspace(source[pos])) {
            ++pos;
            continue;
        }

        if (source[pos] == '"') {
            ++pos;
            std::string strVal;
            while (pos < source.size() && source[pos] != '"') {
                strVal += source[pos++];
            }
            ++pos;
            tokens.emplace_back(strVal, "stringConstant");
            continue;
        }

        if (symbols.find(source[pos]) != std::string::npos) {
            std::string sym(1, source[pos++]);
            tokens.emplace_back(sym, "symbol");
            continue;
        }

        if (std::isdigit(source[pos])) {
            std::string intVal;
            while (pos < source.size() && std::isdigit(source[pos])) {
                intVal += source[pos++];
            }
            tokens.emplace_back(intVal, "integerConstant");
            continue;
        }

        if (std::isalpha(source[pos]) || source[pos] == '_') {
            std::string ident;
            while (pos < source.size() && (std::isalnum(source[pos]) || source[pos] == '_')) {
                ident += source[pos++];
            }
            if (std::find(keywords.begin(), keywords.end(), ident) != keywords.end()) {
                tokens.emplace_back(ident, "keyword");
            } else {
                tokens.emplace_back(ident, "identifier");
            }
            continue;
        }

        ++pos; // skip unknown
    }
}
