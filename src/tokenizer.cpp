//
// Created by patri on 29.10.2026.
//

#include "tokenizer.h"
#include <cctype>
#include <algorithm>

Tokenizer::Tokenizer(const std::string& delimiter_, bool convTOlow_)
    : delimiter(delimiter_), convTOlow(convTOlow_) {
}

bool Tokenizer::isDelimiter(char c) const {
    if (delimiter.find(c) != std::string::npos) {
        return true;
    }
    return false;
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

const std::string& Tokenizer::getDelimiter() const {
    return delimiter;
}

bool Tokenizer::isLowenable() const {
    return convTOlow;
}

int Tokenizer::countTokens(const std::string& text) const {
    auto all = tokenize(text);
    return static_cast<int>(all.size());
}

std::vector<std::string> Tokenizer::extractWordsOnly(const std::string& text) const {
    std::vector<std::string> res;
    auto all = tokenize(text);
    for (const auto& p : all) {
        res.push_back(p.first);
    }
    return res;
}

std::vector<std::string> Tokenizer::extractUniqueWords(const std::string& text) const {
    std::vector<std::string> uniqueWords;
    auto all = tokenize(text);
    for (const auto& p : all) {
        const std::string& w = p.first;
        if (std::find(uniqueWords.begin(), uniqueWords.end(), w) == uniqueWords.end()) {
            uniqueWords.push_back(w);
        }
    }
    std::sort(uniqueWords.begin(), uniqueWords.end());
    return uniqueWords;
}

std::vector<std::string> Tokenizer::filterByMinLength(const std::string& text, std::size_t minLen) const {
    std::vector<std::string> res;
    auto all = tokenize(text);
    for (const auto& p : all) {
        if (p.first.size() >= minLen) {
            res.push_back(p.first);
        }
    }
    return res;
}

bool Tokenizer::isStopWord(const std::string& word) const {
    std::string w = cleanTok(word);
    static const std::vector<std::string> stops = {
        "the", "and", "or", "in", "on", "at", "to", "of", "for",
        "with", "by", "from", "as", "is", "are", "was", "be", "a", "an"
    };
    for (const auto& s : stops) {
        if (w == s) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> Tokenizer::removeStopWords(const std::string& text) const {
    std::vector<std::string> res;
    auto all = tokenize(text);
    for (const auto& p : all) {
        if (!isStopWord(p.first)) {
            res.push_back(p.first);
        }
    }
    return res;
}

std::string Tokenizer::normalizeWord(const std::string& word) const {
    std::string raw;
    for (char c : word) {
        if (!isDelimiter(c)) {
            raw.push_back(c);
        }
    }
    return cleanTok(raw);
}

std::string Tokenizer::stripDelimiters(const std::string& text) const {
    std::string res;
    for (char c : text) {
        if (isDelimiter(c)) {
            if (!res.empty() && res.back() != ' ') {
                res.push_back(' ');
            }
        } else {
            res.push_back(c);
        }
    }
    if (!res.empty() && res.back() == ' ') {
        res.pop_back();
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

std::string Tokenizer::findLongestToken(const std::string& text) const {
    auto all = tokenize(text);
    std::string longest;
    for (const auto& p : all) {
        if (p.first.size() > longest.size()) {
            longest = p.first;
        }
    }
    return longest;
}

double Tokenizer::getAverageTokenLength(const std::string& text) const {
    auto all = tokenize(text);
    if (all.empty()) {
        return 0.0;
    }
    std::size_t sum = 0;
    for (const auto& p : all) {
        sum += p.first.size();
    }
    return static_cast<double>(sum) / static_cast<double>(all.size());
}

std::string Tokenizer::analyzeSample(const std::string& sample) const {
    auto faraStop = removeStopWords(sample);
    std::string out = "Tokenizer [low=" + std::to_string(isLowenable())
                    + ", delimLen=" + std::to_string(getDelimiter().size()) + "]\n";
    out += " - total=" + std::to_string(countTokens(sample))
         + " words=" + std::to_string(extractWordsOnly(sample).size())
         + " unique=" + std::to_string(extractUniqueWords(sample).size())
         + " min4=" + std::to_string(filterByMinLength(sample, 4).size()) + "\n";
    out += " - stop('the')=" + std::to_string(isStopWord("the"))
         + " noStop=" + std::to_string(faraStop.size())
         + " joined=" + joinTokens(faraStop, "-") + "\n";
    out += " - norm=" + normalizeWord(" Test! ")
         + " strip=" + stripDelimiters(sample)
         + " long=" + findLongestToken(sample)
         + " avg=" + std::to_string(getAverageTokenLength(sample));
    return out;
}

std::ostream& operator<<(std::ostream& os, const Tokenizer& tok) {
    os << "Tokenizer [delimiter: \"" << tok.delimiter << "\" ~ lowercase"
       << (tok.convTOlow ? "true" : "false") << "]";
    return os;
}