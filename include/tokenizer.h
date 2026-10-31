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

    friend std::ostream& operator<<(std::ostream& os, const Tokenizer& tok);
};

#endif //OOP_TOKENIZER_H