//
// Created by patri on 29.10.2026.
//

#ifndef OOP_INDEX_H
#define OOP_INDEX_H

#include "doc.h"
#include "tokenizer.h"
#include "entry.h"

#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>

class InvIndex {
private:
    Tokenizer tokenizer;
    std::vector<Document> documents;
    std::unordered_map<std::string, std::vector<Entry>> dictionary;

    [[nodiscard]] static int leveninDist(const std::string& s1, const std::string& s2);

public:

    explicit InvIndex(Tokenizer tok);

    void addDoc(const Document& doc);

    [[nodiscard]] std::vector<int> searchExact(const std::string& termn) const;

    [[nodiscard]] std::vector<std::string> fuzzySugg(const std::string& typo, int maxDist) const;

    [[nodiscard]] std::size_t getDocCount() const;
    [[nodiscard]] const std::vector<Document>& getDocuments() const;

    friend std::ostream& operator<< (std::ostream& os, const InvIndex& idx);
};

#endif //OOP_INDEX_H
