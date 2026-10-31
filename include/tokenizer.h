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

    [[nodiscard]] bool isDelimiter(char c) const;
    [[nodiscard]] std::string cleanTok(const std::string& raw) const;

public:
    Tokenizer(std::string delimiter, bool convTOlow);

    [[nodiscard]] std::vector<std::pair<std::string, int>> tokenize(const std::string& text) const;

    [[nodiscard]] const std::string& getDelimiter() const;
    [[nodiscard]] bool isLowenable() const;

    friend std::ostream& operator<<(std::ostream& os, const Tokenizer& tok);
};

#endif //OOP_TOKENIZER_H
