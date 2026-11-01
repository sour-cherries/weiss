//
// Created by patri on 29.10.2026.
//

#include "tokenizer.h"
#include <cctype>
#include <algorithm>

Tokenizer::Tokenizer(const std::string& delimiter, bool convTOlow)
    : delimiter(delimiter), convTOlow(convTOlow) {
}

bool Tokenizer::isDelimiter(char c) const {
     return delimiter.find(c) != std::string::npos;
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
     std::string current;
     int wordPos = 0;

     for (char ch : text) {
          if (isDelimiter(ch)) {
               if (!current.empty()) {
                    tokens.emplace_back(cleanTok(current), wordPos);
                    current.clear();
                    wordPos++;
               }
          } else {
               current.push_back(ch);
          }
     }

     if (!current.empty()) {
          tokens.emplace_back(cleanTok(current), wordPos);
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
          "the", "and", "or", "in", "on", "at", "to", "of", "for", "with", "is", "are", "a", "an"
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

std::ostream& operator<<(std::ostream& os, const Tokenizer& tok) {
     os << "Tokenizer [delimiter: \"" << tok.delimiter << "\" ~ lowercase: " << (tok.convTOlow ? "true" : "false") << "]";
     return os;
}