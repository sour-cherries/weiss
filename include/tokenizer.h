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

    bool isDelimiter(char c) const;
    std::string cleanTok(const std::string& raw) const;

public:
    Tokenizer(const std::string& delimiter, bool convTOlow);

    std::vector<std::pair<std::string, int>> tokenize(const std::string& text) const;

    const std::string& getDelimiter() const;
    bool isLowenable() const;

    int countTokens(const std::string& text) const;
    std::vector<std::string> extractWordsOnly(const std::string& text) const;
    std::vector<std::string> extractUniqueWords(const std::string& text) const;
    std::vector<std::string> filterByMinLength(const std::string& text, std::size_t minLen) const;
    bool isStopWord(const std::string& word) const;
    std::vector<std::string> removeStopWords(const std::string& text) const;
    std::string normalizeWord(const std::string& word) const;
    std::string stripDelimiters(const std::string& text) const;
    static std::string joinTokens(const std::vector<std::string>& words, const std::string& sep);
    std::string findLongestToken(const std::string& text) const;
    double getAverageTokenLength(const std::string& text) const;

    std::string analyzeSample(const std::string& sample) const;

    friend std::ostream& operator<<(std::ostream& os, const Tokenizer& tok);
};

#endif //OOP_TOKENIZER_H