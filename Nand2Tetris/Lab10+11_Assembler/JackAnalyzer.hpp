#ifndef JACKANALYZER_HPP
#define JACKANALYZER_HPP

#include "CompilationEngine.hpp"

#include <cstring>
#include <stdexcept>

class JackAnalyzer {
public:
    JackAnalyzer(const std::string& inputFile, const std::string& outputFile);
    void analyze();

private:
    std::unique_ptr<JackTokenizer> tokenizer;
    std::unique_ptr<CompilationEngine> engine;
};

#endif