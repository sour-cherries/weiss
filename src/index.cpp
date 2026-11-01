#include "index.h"
#include <algorithm>

#include "index.h"
#include <algorithm>
#include <cmath>

InvIndex::InvIndex(const Tokenizer& tok)
    : tokenizer(tok) {
}

const Document* InvIndex::findDocById(int docID) const {
    for (const auto& doc : documents) {
        if (doc.getId() == docID) {
            return &doc;
        }
    }
    return nullptr;
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

    const std::string& cleanTerm = tokens.front().first;
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

    const std::string& cleanTerm = tokens.front().first;

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
    const std::string& cleanTerm = searchTokens.front().first;

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

    const Document* targetDoc = findDocById(docID);
    if (!targetDoc) {
        return "";
    }

    auto docTokens = tokenizer.tokenize(targetDoc->getContent());
    int totalWords = static_cast<int>(docTokens.size());
    if (totalWords == 0) {
        return "";
    }

    const auto& positions = targetEntry->getPos();
    int firstPos = positions.front();
    int proc = (firstPos * 100) / totalWords;
    std::string zona = "mijloc";
    if (proc < 33) {
        zona = "inceput";
    } else if (proc > 66) {
        zona = "final";
    }

    std::string posStr = "[";
    std::size_t limit = std::min<std::size_t>(positions.size(), 4);
    for (std::size_t i = 0; i < limit; ++i) {
        posStr += "#" + std::to_string(positions[i] + 1);
        if (i + 1 < limit) {
            posStr += ", ";
        }
    }
    if (positions.size() > limit) {
        posStr += ", +" + std::to_string(positions.size() - limit) + " altele";
    }
    posStr += "]";

    int start = std::max(0, firstPos - 3);
    int end = std::min(totalWords - 1, firstPos + 3);

    std::string snippet = "\"...";
    for (int i = start; i <= end; ++i) {
        if (i == firstPos) {
            snippet += " [" + docTokens[static_cast<std::size_t>(i)].first + "]";
        } else {
            snippet += " " + docTokens[static_cast<std::size_t>(i)].first;
        }
    }
    snippet += " ...\"";

    return std::to_string(targetEntry->getFreq()) + " aparitii | pozitii: "
           + posStr + " (~" + std::to_string(proc) + "% " + zona + ") | context: " + snippet;
}

std::string InvIndex::bestTypoMatch(const std::string& typo, int maxDist) const {
    auto tokens = tokenizer.tokenize(typo);
    if (tokens.empty()) {
        return "";
    }
    const std::string& cleanTerm = tokens.front().first;

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
    const std::string& cleanPref = tokens.front().first;

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

std::vector<int> InvIndex::searchAND(const std::vector<std::string>& terms) const {
    if (terms.empty()) {
        return {};
    }

    std::vector<int> result = searchExact(terms[0]);
    for (std::size_t i = 1; i < terms.size(); ++i) {
        if (result.empty()) {
            break;
        }
        std::vector<int> current = searchExact(terms[i]);
        std::vector<int> intersection;
        for (int id : result) {
            if (std::find(current.begin(), current.end(), id) != current.end()) {
                intersection.push_back(id);
            }
        }
        result = intersection;
    }
    return result;
}

std::vector<int> InvIndex::searchOR(const std::vector<std::string>& terms) const {
    std::vector<int> result;
    for (const auto& term : terms) {
        std::vector<int> docs = searchExact(term);
        for (int id : docs) {
            if (std::find(result.begin(), result.end(), id) == result.end()) {
                result.push_back(id);
            }
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}

std::vector<int> InvIndex::searchNOT(const std::string& includeTerm, const std::string& excludeTerm) const {
    std::vector<int> incDocs = searchExact(includeTerm);
    std::vector<int> excDocs = searchExact(excludeTerm);
    std::vector<int> result;

    for (int id : incDocs) {
        if (std::find(excDocs.begin(), excDocs.end(), id) == excDocs.end()) {
            result.push_back(id);
        }
    }
    return result;
}

std::vector<int> InvIndex::searchQuery(const std::string& expr, std::vector<std::string>& termsUsed) const {
    auto tokens = tokenizer.tokenize(expr);
    std::vector<int> result;
    std::string op = "or";
    bool firstWord = true;

    for (const auto& pair : tokens) {
        const std::string& word = pair.first;
        if (word == "and" || word == "or") {
            op = word;
            continue;
        }

        std::string used;
        std::vector<int> currentDocs = smartSearch(word, used);
        if (!used.empty()) {
            termsUsed.push_back(used);
        }

        if (firstWord) {
            result = currentDocs;
            firstWord = false;
        } else if (op == "and") {
            std::vector<int> temp;
            for (int id : result) {
                if (std::find(currentDocs.begin(), currentDocs.end(), id) != currentDocs.end()) {
                    temp.push_back(id);
                }
            }
            result = temp;
        } else {
            for (int id : currentDocs) {
                if (std::find(result.begin(), result.end(), id) == result.end()) {
                    result.push_back(id);
                }
            }
        }
    }

    std::sort(result.begin(), result.end());
    return result;
}

std::vector<int> InvIndex::searchPhrase(const std::string& firstWord, const std::string& secondWord) const {
    return searchProximity(firstWord, secondWord, 1);
}

std::vector<int> InvIndex::searchProximity(const std::string& w1, const std::string& w2, int maxDist) const {
    std::vector<int> result;
    auto t1 = tokenizer.tokenize(w1);
    auto t2 = tokenizer.tokenize(w2);
    if (t1.empty() || t2.empty() || maxDist <= 0) {
        return result;
    }

    auto it1 = dictionary.find(t1.front().first);
    auto it2 = dictionary.find(t2.front().first);
    if (it1 == dictionary.end() || it2 == dictionary.end()) {
        return result;
    }

    for (const auto& e1 : it1->second) {
        for (const auto& e2 : it2->second) {
            if (e1.getDocID() == e2.getDocID()) {
                if (e1.isFollowedBy(e2, maxDist)) {
                    result.push_back(e1.getDocID());
                }
            }
        }
    }
    return result;
}

std::vector<std::pair<int, double>> InvIndex::searchRanked(const std::string& term) const {
    std::vector<std::pair<int, double>> ranked;
    auto tokens = tokenizer.tokenize(term);
    if (tokens.empty() || documents.empty()) {
        return ranked;
    }

    const std::string& cleanTerm = tokens.front().first;
    auto it = dictionary.find(cleanTerm);
    if (it == dictionary.end()) {
        return ranked;
    }

    double docsWithTerm = static_cast<double>(it->second.size());
    double totalDocs = static_cast<double>(documents.size());
    double idf = std::log(1.0 + (totalDocs / docsWithTerm));

    for (const auto& entry : it->second) {
        int docID = entry.getDocID();
        int totalWords = getDocTotalWords(docID);
        if (totalWords > 0) {
            double tf = entry.getRelativeDensity(totalWords);
            ranked.emplace_back(docID, tf * idf);
        }
    }

    std::sort(ranked.begin(), ranked.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
    });

    return ranked;
}

int InvIndex::getTermTotalFreq(const std::string& term) const {
    auto tokens = tokenizer.tokenize(term);
    if (tokens.empty()) {
        return 0;
    }
    auto it = dictionary.find(tokens.front().first);
    if (it == dictionary.end()) {
        return 0;
    }
    int total = 0;
    for (const auto& entry : it->second) {
        total += entry.getFreq();
    }
    return total;
}

int InvIndex::getTermDocFreq(const std::string& term) const {
    auto tokens = tokenizer.tokenize(term);
    if (tokens.empty()) {
        return 0;
    }
    auto it = dictionary.find(tokens.front().first);
    if (it == dictionary.end()) {
        return 0;
    }
    return static_cast<int>(it->second.size());
}

std::string InvIndex::getMostFrequentTerm() const {
    std::string bestTerm;
    int maxFreq = -1;

    for (const auto& item : dictionary) {
        int sum = 0;
        for (const auto& entry : item.second) {
            sum += entry.getFreq();
        }
        if (sum > maxFreq) {
            maxFreq = sum;
            bestTerm = item.first;
        }
    }
    return bestTerm;
}

int InvIndex::getDocUniqueTerms(int docID) const {
    int count = 0;
    for (const auto& item : dictionary) {
        for (const auto& entry : item.second) {
            if (entry.getDocID() == docID) {
                count++;
                break;
            }
        }
    }
    return count;
}

int InvIndex::getDocTotalWords(int docID) const {
    const Document* doc = findDocById(docID);
    if (!doc) {
        return 0;
    }
    return tokenizer.countTokens(doc->getContent());
}

std::vector<std::string> InvIndex::getTopTermsInDoc(int docID, std::size_t topN) const {
    std::vector<std::pair<std::string, int>> freqs;
    for (const auto& item : dictionary) {
        for (const auto& entry : item.second) {
            if (entry.getDocID() == docID) {
                freqs.emplace_back(item.first, entry.getFreq());
                break;
            }
        }
    }

    std::sort(freqs.begin(), freqs.end(), [](const auto& a, const auto& b) {
        if (a.second != b.second) {
            return a.second > b.second;
        }
        return a.first < b.first;
    });

    std::vector<std::string> res;
    std::size_t limit = std::min(topN, freqs.size());
    for (std::size_t i = 0; i < limit; ++i) {
        res.push_back(freqs[i].first);
    }
    return res;
}

std::string InvIndex::getIndexSummary() const {
    return "Docs: " + std::to_string(documents.size())
           + " | Vocab: " + std::to_string(dictionary.size())
           + " | Top: " + getMostFrequentTerm();
}

std::size_t InvIndex::getDocCount() const {
    return documents.size();
}

std::size_t InvIndex::getVocabularySize() const {
    return dictionary.size();
}

const std::vector<Document>& InvIndex::getDocuments() const {
    return documents;
}

std::ostream& operator<<(std::ostream& os, const InvIndex& idx) {
    os << "=== Inv Index (Total documente: " << idx.documents.size() << ", Term unici: " << idx.dictionary.size() << ") ===\n";
    os << "Tokenizer: " << idx.tokenizer << "\n";

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