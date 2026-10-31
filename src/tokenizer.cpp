//
// Created by patri on 29.10.2026.
//

#include "tokenizer.h"
#include <utility>
#include <cctype>

Tokenizer::Tokenizer(std::string delimiter, bool convTOlow):
     delimiter(std::move(delimiter)), convTOlow(convTOlow) {}

bool Tokenizer::isDelimiter(char c) const {
     return delimiter.find(c) != std::string::npos;
}

std::string Tokenizer::cleanTok(const std::string& raw) const {
     if (!convTOlow) {
          return raw;
     }

     std::string result = raw;
     for (char&ch : result) {
          ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
     }

     return result;
}

std::vector<std::pair<std::string, int>> Tokenizer::tokenize(const std::string& text) const {
     std::vector<std::pair<std::string, int>> tokens;
     std::string current;
     int wordPos = 0;

     for (char ch: text) {
          if (isDelimiter(ch)) {
               if (!current.empty()) {
                    tokens.emplace_back(cleanTok(current), wordPos);
                    current.clear();
                    wordPos++;
               }
          }
          else {
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

std::ostream& operator<<(std::ostream& os, const Tokenizer& tok) {
     os << "Tokenizer [delimiter: \"" << tok.delimiter << "\" ~ lowercase" << (tok.convTOlow ? "true" : "false") << "]";
     return os;
}