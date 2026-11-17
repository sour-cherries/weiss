//
// Created by patri on 29.10.2026.
//

#ifndef OOP_TOKENIZER_H
#define OOP_TOKENIZER_H

#include <string>
#include <vector>
#include <utility>
#include <iostream>

class Tokenizer {
private:
    std::string delimiter;
    bool convTOlow;
    std::vector<std::string> stopWords;

    bool isDelimiter(char c) const;
    std::string cleanTok(const std::string& raw) const;

public:
    Tokenizer(const std::string& delimiter, bool convTOlow, const std::vector<std::string>& stopWords);

    std::vector<std::pair<std::string, int>> tokenize(const std::string& text) const;
    int countTokens(const std::string& text) const;

    std::string normalizeWord(const std::string& word) const;
    bool isStopWord(const std::string& word) const;
    std::vector<std::string> removeStopWords(const std::string& text) const;
    static std::string joinTokens(const std::vector<std::string>& words, const std::string& sep);

    std::string analyzeSample(const std::string& sample, const std::string& sep) const;

    friend std::ostream& operator<<(std::ostream& os, const Tokenizer& tok);
};

#endif //OOP_TOKENIZER_H