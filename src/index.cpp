//
// Created by patri on 29.10.2026.
//

#include "index.h"
#include <algorithm>

InvIndex::InvIndex(const Tokenizer& tok)
    : tokenizer(tok) {
}

void InvIndex::addDoc(const Document& doc) {
    documents.push_back(doc);

    std::vector<std::pair<std::string, int>> tokens = tokenizer.tokenize(doc.getContent());

    for (const auto& tokenPair : tokens) {
        const std::string& word = tokenPair.first;
        int pos = tokenPair.second;

        std::vector<Entry>& entries = dictionary[word];
        bool foundDoc = false;

        for (auto& entry : entries) {
            if (entry.getDocID() == doc.getId()) {
                entry.addPos(pos);
                foundDoc = true;
                break;
            }
        }

        if (!foundDoc) {
            entries.emplace_back(doc.getId(), pos);
        }
    }
}

std::vector<int> InvIndex::searchExact(const std::string& term) const {
    std::vector<int> result;

    auto tokens = tokenizer.tokenize(term);
    if (tokens.empty()) {
        return result;
    }

    std::string cleanTerm = tokens.front().first;
    auto it = dictionary.find(cleanTerm);
    if (it != dictionary.end()) {
        for (const auto& entry : it->second) {
            result.push_back(entry.getDocID());
        }
    }
    return result;
}

int InvIndex::leveninDist(const std::string& s1, const std::string& s2) {
    const std::size_t m = s1.size();
    const std::size_t n = s2.size();

    std::vector<std::vector<int>> dp(m + 1, std::vector<int>(n + 1, 0));

    for (std::size_t i = 0; i <= m; ++i) {
        dp[i][0] = static_cast<int>(i);
    }

    for (std::size_t j = 1; j <= n; ++j) {
        dp[0][j] = static_cast<int>(j);
    }

    for (std::size_t i = 1; i <= m; ++i) {
        for (std::size_t j = 1; j <= n; ++j) {
            if (s1[i - 1] == s2[j - 1]) {
                dp[i][j] = dp[i - 1][j - 1];
            } else {
                dp[i][j] = 1 + std::min({dp[i - 1][j], dp[i][j - 1], dp[i - 1][j - 1]});
            }
        }
    }

    return dp[m][n];
}

std::vector<std::string> InvIndex::fuzzySugg(const std::string& typo, int maxDist) const {
    std::vector<std::string> suggestions;
    auto tokens = tokenizer.tokenize(typo);
    if (tokens.empty()) {
        return suggestions;
    }

    std::string cleanTerm = tokens.front().first;

    for (const auto& item : dictionary) {
        const std::string& indexWord = item.first;
        if (leveninDist(cleanTerm, indexWord) <= maxDist) {
            suggestions.push_back(indexWord);
        }
    }
    return suggestions;
}

std::size_t InvIndex::getDocCount() const {
    return documents.size();
}

const std::vector<Document>& InvIndex::getDocuments() const {
    return documents;
}

std::ostream& operator<<(std::ostream& os, const InvIndex& idx) {
    os << "=== Inv Index (Total documente: " << idx.documents.size() << ", Term unici: " << idx.dictionary.size() << ") ===\n";

    os << "~~ Documente inregistrate ~~\n";
    for (const auto& doc : idx.documents) {
        os << " " << doc << std::endl;
    }

    os << "~~ Index Term ~~\n";
    for (const auto& item : idx.dictionary) {
        os << " Term: '" << item.first << "' -> ";
        for (const auto& entry : item.second) {
            os << entry << " ";
        }
        os << std::endl;
    }
    return os;
}
