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
#include <utility>

class InvIndex {
private:
    Tokenizer tokenizer;
    std::vector<Document> documents;
    std::unordered_map<std::string, std::vector<Entry>> dictionary;

    static int leveninDist(const std::string& s1, const std::string& s2);
    const Document* findDocById(int docID) const;

public:
    explicit InvIndex(const Tokenizer& tok);

    void addDoc(const Document& doc);

    std::vector<int> searchExact(const std::string& term) const;
    std::vector<std::string> fuzzySugg(const std::string& typo, int maxDist) const;

    std::string locateTerm(int docID, const std::string& term) const;
    std::string bestTypoMatch(const std::string& typo, int maxDist) const;
    std::vector<std::string> autocomplete(const std::string& prefix) const;
    std::vector<int> smartSearch(const std::string& query, std::string& usedTerm) const;

    std::vector<int> searchAND(const std::vector<std::string>& terms) const;
    std::vector<int> searchOR(const std::vector<std::string>& terms) const;
    std::vector<int> searchNOT(const std::string& includeTerm, const std::string& excludeTerm) const;
    std::vector<int> searchQuery(const std::string& expr, std::vector<std::string>& termsUsed) const;

    std::vector<int> searchPhrase(const std::string& firstWord, const std::string& secondWord) const;
    std::vector<int> searchProximity(const std::string& w1, const std::string& w2, int maxDist) const;
    std::vector<std::pair<int, double>> searchRanked(const std::string& term) const;

    int getTermTotalFreq(const std::string& term) const;
    int getTermDocFreq(const std::string& term) const;
    std::string getMostFrequentTerm() const;
    int getDocUniqueTerms(int docID) const;
    int getDocTotalWords(int docID) const;
    std::vector<std::string> getTopTermsInDoc(int docID, std::size_t topN) const;
    std::string getIndexSummary() const;
    std::string runDiagnostics() const;

    std::size_t getDocCount() const;
    std::size_t getVocabularySize() const;
    const std::vector<Document>& getDocuments() const;

    friend std::ostream& operator<<(std::ostream& os, const InvIndex& idx);
};

#endif //OOP_INDEX_H
