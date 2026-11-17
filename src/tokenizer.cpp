//
// Created by patri on 29.10.2026.
//

#include "tokenizer.h"

#include <algorithm>
#include <cctype>

Tokenizer::Tokenizer(const std::string& delimiter_, bool convTOlow_, const std::vector<std::string>& stopWords_)
    : delimiter(delimiter_), convTOlow(convTOlow_) {
    for (const auto& w : stopWords_) {
        stopWords.push_back(cleanTok(w));
    }
}

bool Tokenizer::isDelimiter(char c) const {
    return std::isspace(static_cast<unsigned char>(c)) != 0 || delimiter.find(c) != std::string::npos;
}

std::string Tokenizer::cleanTok(const std::string& raw) const {
    if (!convTOlow) {
        return raw;
    }
    std::string result = raw;
    for (char& ch : result) {
        ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
    }
    return result;
}

std::vector<std::pair<std::string, int>> Tokenizer::tokenize(const std::string& text) const {
    std::vector<std::pair<std::string, int>> tokens;
    std::string currentWord;
    int wordPosition = 0;

    for (char ch : text) {
        if (isDelimiter(ch)) {
            if (!currentWord.empty()) {
                tokens.emplace_back(cleanTok(currentWord), wordPosition);
                wordPosition++;
                currentWord.clear();
            }
        } else {
            currentWord.push_back(ch);
        }
    }

    if (!currentWord.empty()) {
        tokens.emplace_back(cleanTok(currentWord), wordPosition);
    }

    return tokens;
}

int Tokenizer::countTokens(const std::string& text) const {
    return static_cast<int>(tokenize(text).size());
}

std::string Tokenizer::normalizeWord(const std::string& word) const {
    auto tokens = tokenize(word);
    if (tokens.empty()) {
        return "";
    }
    return tokens.front().first;
}

bool Tokenizer::isStopWord(const std::string& word) const {
    return std::find(stopWords.begin(), stopWords.end(), cleanTok(word)) != stopWords.end();
}

std::vector<std::string> Tokenizer::removeStopWords(const std::string& text) const {
    std::vector<std::string> res;
    for (const auto& p : tokenize(text)) {
        if (!isStopWord(p.first)) {
            res.push_back(p.first);
        }
    }
    return res;
}

std::string Tokenizer::joinTokens(const std::vector<std::string>& words, const std::string& sep) {
    std::string res;
    for (std::size_t i = 0; i < words.size(); ++i) {
        res += words[i];
        if (i + 1 < words.size()) {
            res += sep;
        }
    }
    return res;
}

std::string Tokenizer::analyzeSample(const std::string& sample, const std::string& sep) const {
    std::vector<std::string> faraStop = removeStopWords(sample);
    return "Tokenizer: total=" + std::to_string(countTokens(sample))
         + " faraStop=" + std::to_string(faraStop.size())
         + " -> " + joinTokens(faraStop, sep)
         + " | primul=" + normalizeWord(sample);
}

std::ostream& operator<<(std::ostream& os, const Tokenizer& tok) {
    os << "Tokenizer [delimiter: \"" << tok.delimiter << "\" ~ lowercase: "
       << (tok.convTOlow ? "true" : "false")
       << " ~ stopWords: " << tok.stopWords.size() << "]";
    return os;
}