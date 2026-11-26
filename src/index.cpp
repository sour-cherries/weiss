//
// Created by patri on 29.10.2026.
//

#include "index.h"
#include "exceptions.h"

#include <algorithm>
#include <cmath>
#include <map>

InvIndex::InvIndex(const Tokenizer& tok)
    : tokenizer(tok) {
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

int InvIndex::sumFreq(const std::vector<Entry>& entries) {
    int total = 0;
    for (const auto& entry : entries) {
        total += entry.getFreq();
    }
    return total;
}

const Entry* InvIndex::findEntryInDoc(const std::vector<Entry>& entries, int docID) {
    for (const auto& entry : entries) {
        if (entry.getDocID() == docID) {
            return &entry;
        }
    }
    return nullptr;
}

const Document* InvIndex::findDocById(int docID) const {
    for (const auto& doc : documents) {
        if (doc.getId() == docID) {
            return &doc;
        }
    }
    return nullptr;
}

const std::vector<Entry>* InvIndex::findEntries(const std::string& term) const {
    std::string clean = tokenizer.normalizeWord(term);
    if (clean.empty()) {
        return nullptr;
    }
    auto it = dictionary.find(clean);
    if (it == dictionary.end()) {
        return nullptr;
    }
    return &it->second;
}

void InvIndex::addDoc(const Document& doc) {
    if (findDocById(doc.getId()) != nullptr) {
        throw IndexError("exista deja un document cu id " + std::to_string(doc.getId()));
    }
    documents.push_back(doc);

    std::vector<std::pair<std::string, int>> tokens = tokenizer.tokenize(doc.getContent());

    for (const auto& tokenPair : tokens) {
        const std::string& word = tokenPair.first;
        int pos = tokenPair.second;

        if (tokenizer.isStopWord(word)) {
            continue;
        }

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
    const std::vector<Entry>* entries = findEntries(term);
    if (entries == nullptr) {
        return result;
    }
    for (const auto& entry : *entries) {
        result.push_back(entry.getDocID());
    }
    std::sort(result.begin(), result.end());
    return result;
}

std::vector<std::string> InvIndex::fuzzySugg(const std::string& typo, int maxDist) const {
    std::vector<std::string> suggestions;
    std::string cleanTerm = tokenizer.normalizeWord(typo);
    if (cleanTerm.empty()) {
        return suggestions;
    }

    for (const auto& item : dictionary) {
        if (leveninDist(cleanTerm, item.first) <= maxDist) {
            suggestions.push_back(item.first);
        }
    }
    std::sort(suggestions.begin(), suggestions.end());
    return suggestions;
}

std::string InvIndex::bestTypoMatch(const std::string& typo, int maxDist) const {
    std::string cleanTerm = tokenizer.normalizeWord(typo);
    if (cleanTerm.empty()) {
        return "";
    }

    std::string bestWord;
    int bestDist = maxDist + 1;
    int bestTotalFreq = -1;

    for (const auto& item : dictionary) {
        int dist = leveninDist(cleanTerm, item.first);

        if (dist <= maxDist) {
            int totalFreq = sumFreq(item.second);

            if (dist < bestDist
                || (dist == bestDist && totalFreq > bestTotalFreq)
                || (dist == bestDist && totalFreq == bestTotalFreq && item.first < bestWord)) {
                bestDist = dist;
                bestTotalFreq = totalFreq;
                bestWord = item.first;
            }
        }
    }
    return bestWord;
}

std::vector<std::string> InvIndex::autocomplete(const std::string& prefix) const {
    std::vector<std::string> matches;
    std::string cleanPref = tokenizer.normalizeWord(prefix);
    if (cleanPref.empty()) {
        return matches;
    }

    for (const auto& item : dictionary) {
        const std::string& word = item.first;
        if (word.size() >= cleanPref.size() && word.compare(0, cleanPref.size(), cleanPref) == 0) {
            matches.push_back(word);
        }
    }
    std::sort(matches.begin(), matches.end());
    return matches;
}

std::vector<int> InvIndex::smartSearch(const std::string& query, int maxDist, std::string& usedTerm) const {
    usedTerm = tokenizer.normalizeWord(query);
    if (usedTerm.empty() || tokenizer.isStopWord(usedTerm)) {
        usedTerm = "";
        return {};
    }

    std::vector<int> exact = searchExact(usedTerm);
    if (!exact.empty()) {
        return exact;
    }

    std::string corrected = bestTypoMatch(usedTerm, maxDist);
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

std::vector<int> InvIndex::searchQuery(const std::string& expr, int maxDist,
                                       std::vector<std::string>& termsUsed) const {
    auto tokens = tokenizer.tokenize(expr);
    std::vector<int> result;
    std::string op = "or";
    bool firstWord = true;

    for (const auto& pair : tokens) {
        const std::string& word = pair.first;
        if (word == "and" || word == "or" || word == "not") {
            op = word;
            continue;
        }

        std::string used;
        std::vector<int> currentDocs = smartSearch(word, maxDist, used);
        if (used.empty()) {
            continue;
        }
        termsUsed.push_back(used);

        if (firstWord) {
            if (op == "not") {
                throw CommandError("cautarea nu poate incepe cu not");
            }
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
        } else if (op == "not") {
            std::vector<int> temp;
            for (int id : result) {
                if (std::find(currentDocs.begin(), currentDocs.end(), id) == currentDocs.end()) {
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
        op = "or";
    }

    std::sort(result.begin(), result.end());
    return result;
}

std::vector<int> InvIndex::searchPhrase(const std::string& firstWord, const std::string& secondWord,
                                        int offset) const {
    std::vector<int> result;
    const std::vector<Entry>* entries1 = findEntries(firstWord);
    const std::vector<Entry>* entries2 = findEntries(secondWord);
    if (entries1 == nullptr || entries2 == nullptr) {
        return result;
    }

    for (const auto& e1 : *entries1) {
        const Entry* e2 = findEntryInDoc(*entries2, e1.getDocID());
        if (e2 != nullptr && !e1.getPhraseStarts(*e2, offset).empty()) {
            result.push_back(e1.getDocID());
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}

std::vector<int> InvIndex::searchProximity(const std::string& w1, const std::string& w2, int maxDist) const {
    std::vector<int> result;
    const std::vector<Entry>* entries1 = findEntries(w1);
    const std::vector<Entry>* entries2 = findEntries(w2);
    if (entries1 == nullptr || entries2 == nullptr || maxDist <= 0) {
        return result;
    }

    for (const auto& e1 : *entries1) {
        const Entry* e2 = findEntryInDoc(*entries2, e1.getDocID());
        if (e2 != nullptr && (e1.isFollowedBy(*e2, maxDist) || e2->isFollowedBy(e1, maxDist))) {
            result.push_back(e1.getDocID());
        }
    }
    std::sort(result.begin(), result.end());
    return result;
}

std::vector<std::pair<int, double>> InvIndex::searchRanked(const std::string& query) const {
    std::map<int, double> scores;
    if (documents.empty()) {
        return {};
    }

    double totalDocs = static_cast<double>(documents.size());
    double sumLen = 0.0;
    for (const auto& doc : documents) {
        sumLen += static_cast<double>(getDocTotalWords(doc.getId()));
    }
    double avgLen = sumLen / totalDocs;

    for (const auto& token : tokenizer.tokenize(query)) {
        const std::vector<Entry>* entries = findEntries(token.first);
        if (entries == nullptr) {
            continue;
        }

        double docsWithTerm = static_cast<double>(entries->size());
        double idf = std::log1p(totalDocs / docsWithTerm);

        for (const auto& entry : *entries) {
            int docLen = getDocTotalWords(entry.getDocID());
            scores[entry.getDocID()] += entry.getBM25(idf, docLen, avgLen);
        }
    }

    std::vector<std::pair<int, double>> ranked(scores.begin(), scores.end());
    std::sort(ranked.begin(), ranked.end(), [](const auto& a, const auto& b) {
        if (a.second != b.second) {
            return a.second > b.second;
        }
        return a.first < b.first;
    });
    return ranked;
}

std::string InvIndex::locateTerm(int docID, const std::string& term,
                                 std::size_t radius, std::size_t maxShown) const {
    const std::vector<Entry>* entries = findEntries(term);
    if (entries == nullptr) {
        return "";
    }
    const Entry* entry = findEntryInDoc(*entries, docID);
    const Document* doc = findDocById(docID);
    if (entry == nullptr || doc == nullptr) {
        return "";
    }

    std::string cleanTerm = tokenizer.normalizeWord(term);
    const std::vector<int>& positions = entry->getPos();

    std::string posStr = "[";
    std::size_t limit = std::min(positions.size(), maxShown);
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

    return std::to_string(entry->getFreq()) + " aparitii | linia " + std::to_string(doc->getLineOf(cleanTerm))
           + " | pozitii: " + posStr
           + " (prima: cuvantul " + std::to_string(entry->getFirstPos() + 1)
           + " din " + std::to_string(getDocTotalWords(docID)) + ")"
           + " | context: " + doc->getSnippet(cleanTerm, radius);
}

std::string InvIndex::describeDoc(int docID, const std::vector<std::string>& terms,
                                  std::size_t radius, std::size_t maxShown) const {
    const Document* doc = findDocById(docID);
    if (doc == nullptr) {
        return "";
    }

    std::string res = "[" + doc->getTitle() + "] (" + doc->getPath() + ")\n";
    for (const auto& term : terms) {
        std::string loc = locateTerm(docID, term, radius, maxShown);
        if (!loc.empty()) {
            res += "    * " + term + ": " + loc + "\n";
        }
    }
    return res;
}

int InvIndex::getTermTotalFreq(const std::string& term) const {
    const std::vector<Entry>* entries = findEntries(term);
    if (entries == nullptr) {
        return 0;
    }
    return sumFreq(*entries);
}

int InvIndex::getTermDocFreq(const std::string& term) const {
    const std::vector<Entry>* entries = findEntries(term);
    if (entries == nullptr) {
        return 0;
    }
    return static_cast<int>(entries->size());
}

std::string InvIndex::getMostFrequentTerm() const {
    std::string bestTerm;
    int maxFreq = -1;

    for (const auto& item : dictionary) {
        int sum = sumFreq(item.second);
        if (sum > maxFreq || (sum == maxFreq && item.first < bestTerm)) {
            maxFreq = sum;
            bestTerm = item.first;
        }
    }
    return bestTerm;
}

int InvIndex::getDocUniqueTerms(int docID) const {
    int count = 0;
    for (const auto& item : dictionary) {
        if (findEntryInDoc(item.second, docID) != nullptr) {
            count++;
        }
    }
    return count;
}

int InvIndex::getDocTotalWords(int docID) const {
    const Document* doc = findDocById(docID);
    if (doc == nullptr) {
        return 0;
    }
    return tokenizer.countTokens(doc->getContent());
}

std::vector<std::string> InvIndex::getTopTermsInDoc(int docID, std::size_t topN) const {
    std::vector<std::pair<std::string, int>> freqs;
    for (const auto& item : dictionary) {
        const Entry* entry = findEntryInDoc(item.second, docID);
        if (entry != nullptr) {
            freqs.emplace_back(item.first, entry->getFreq());
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

const std::vector<Document>& InvIndex::getDocuments() const {
    return documents;
}

std::ostream& operator<<(std::ostream& os, const InvIndex& idx) {
    os << "=== Inv Index (Total documente: " << idx.documents.size()
       << ", Term unici: " << idx.dictionary.size() << ") ===\n";
    os << idx.tokenizer << "\n";

    os << "~~ Documente inregistrate ~~\n";
    for (const auto& doc : idx.documents) {
        os << " " << doc << "\n";
    }

    os << "~~ Index Term ~~\n";
    for (const auto& item : idx.dictionary) {
        os << " Term: '" << item.first << "' -> ";
        for (const auto& entry : item.second) {
            os << entry << " ";
        }
        os << "\n";
    }
    return os;
}