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

    static int leveninDist(const std::string& s1, const std::string& s2);

public:
    explicit InvIndex(const Tokenizer& tok);

    void addDoc(const Document& doc);

    std::vector<int> searchExact(const std::string& term) const;

    std::vector<std::string> fuzzySugg(const std::string& typo, int maxDist) const;

    std::string locateTerm(int docID, const std::string& term) const;
    std::string bestTypoMatch(const std::string& typo, int maxDist) const;
    std::vector<std::string> autocomplete(const std::string& prefix) const;
    std::vector<int> smartSearch(const std::string& query, std::string& usedTerm) const;

    std::size_t getDocCount() const;
    const std::vector<Document>& getDocuments() const;

    friend std::ostream& operator<<(std::ostream& os, const InvIndex& idx);
};

#endif //OOP_INDEX_H
