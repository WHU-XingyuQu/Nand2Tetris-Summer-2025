#pragma once
#ifndef JACKTOKENIZER_HPP
#define JACKTOKENIZER_HPP

#include <string>
#include <vector>

class JackTokenizer {
public:
    std::vector<std::pair<std::string, std::string>> tokens;
    size_t token_index = 0;

    JackTokenizer(const std::string& inputFile);

private:
    std::string source;

    void removeComments();
    void tokenize();
};

#endif
