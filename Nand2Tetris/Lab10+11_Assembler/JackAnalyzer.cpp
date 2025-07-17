#include "JackAnalyzer.hpp"

JackAnalyzer::JackAnalyzer(const std::string& inputFile, const std::string& outputFile)
    : tokenizer(std::make_unique<JackTokenizer>(inputFile)),
      engine(std::make_unique<CompilationEngine>(*tokenizer, outputFile)) {}

void JackAnalyzer::analyze() {
    while(tokenizer->hasMoreTokens()){
        tokenizer->advance();
    }
    engine->compileClass();
}
