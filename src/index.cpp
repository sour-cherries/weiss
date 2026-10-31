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

std::string InvIndex::locateTerm(int docID, const std::string& term) const {
    auto searchTokens = tokenizer.tokenize(term);
    if (searchTokens.empty()) {
        return "";
    }
    std::string cleanTerm = searchTokens.front().first;

    auto it = dictionary.find(cleanTerm);
    if (it == dictionary.end()) {
        return "";
    }

    const Entry* targetEntry = nullptr;
    for (const auto& entry : it->second) {
        if (entry.getDocID() == docID) {
            targetEntry = &entry;
            break;
        }
    }
    if (!targetEntry) {
        return "";
    }

    const Document* targetDoc = nullptr;
    for (const auto& doc : documents) {
        if (doc.getId() == docID) {
            targetDoc = &doc;
            break;
        }
    }
    if (!targetDoc) {
        return "";
    }

    auto docTokens = tokenizer.tokenize(targetDoc->getContent());
    int totalWords = static_cast<int>(docTokens.size());
    if (totalWords == 0) {
        return "";
    }

    std::string info = "aparitii: " + std::to_string(targetEntry->getFreq()) + " | locatii: ";
    const auto& positions = targetEntry->getPos();

    for (std::size_t i = 0; i < positions.size(); ++i) {
        int pos = positions[i];
        int proc = (pos * 100) / totalWords;
        std::string zona = "mijloc";
        if (proc < 33) {
            zona = "inceput";
        } else if (proc > 66) {
            zona = "final";
        }

        info += "cuvantul #" + std::to_string(pos + 1) + "/" + std::to_string(totalWords)
              + " (" + std::to_string(proc) + "% - " + zona + ")";
        if (i + 1 < positions.size()) {
            info += ", ";
        }
    }

    int firstPos = positions.front();
    int start = std::max(0, firstPos - 3);
    int end = std::min(totalWords - 1, firstPos + 3);

    info += " | context: \"...";
    for (int i = start; i <= end; ++i) {
        if (i == firstPos) {
            info += " [" + docTokens[static_cast<std::size_t>(i)].first + "]";
        } else {
            info += " " + docTokens[static_cast<std::size_t>(i)].first;
        }
    }
    info += " ...\"";

    return info;
}

std::string InvIndex::bestTypoMatch(const std::string& typo, int maxDist) const {
    auto tokens = tokenizer.tokenize(typo);
    if (tokens.empty()) {
        return "";
    }
    std::string cleanTerm = tokens.front().first;

    std::string bestWord;
    int bestDist = maxDist + 1;
    int bestTotalFreq = -1;

    for (const auto& item : dictionary) {
        const std::string& word = item.first;
        int dist = leveninDist(cleanTerm, word);

        if (dist <= maxDist) {
            int totalFreq = 0;
            for (const auto& entry : item.second) {
                totalFreq += entry.getFreq();
            }

            if (dist < bestDist || (dist == bestDist && totalFreq > bestTotalFreq)) {
                bestDist = dist;
                bestTotalFreq = totalFreq;
                bestWord = word;
            }
        }
    }
    return bestWord;
}

std::vector<std::string> InvIndex::autocomplete(const std::string& prefix) const {
    std::vector<std::string> matches;
    auto tokens = tokenizer.tokenize(prefix);
    if (tokens.empty()) {
        return matches;
    }
    std::string cleanPref = tokens.front().first;

    for (const auto& item : dictionary) {
        const std::string& word = item.first;
        if (word.size() >= cleanPref.size() && word.compare(0, cleanPref.size(), cleanPref) == 0) {
            matches.push_back(word);
        }
    }
    std::sort(matches.begin(), matches.end());
    return matches;
}

std::vector<int> InvIndex::smartSearch(const std::string& query, std::string& usedTerm) const {
    auto tokens = tokenizer.tokenize(query);
    if (tokens.empty()) {
        usedTerm = "";
        return {};
    }
    usedTerm = tokens.front().first;

    std::vector<int> exact = searchExact(usedTerm);
    if (!exact.empty()) {
        return exact;
    }

    std::string corrected = bestTypoMatch(usedTerm, 2);
    if (!corrected.empty()) {
        usedTerm = corrected;
        return searchExact(corrected);
    }

    return {};
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