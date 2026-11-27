//
// Created by patri on 26.11.2026.
//

#include "ranker.h"
#include "exceptions.h"

#include <algorithm>
#include <map>

Ranker::Ranker(const Scorer& s)
    : scorer(s.clone()) {
}

Ranker::Ranker(const Ranker& other)
    : scorer(other.scorer->clone()) {
}

Ranker& Ranker::operator=(Ranker other) {
    swap(*this, other);
    return *this;
}

Ranker::~Ranker() {
}

void swap(Ranker& first, Ranker& second) {
    std::swap(first.scorer, second.scorer);
}

void Ranker::setFrequencyLimit(int limit) {
    FrequencyScorer* freqScorer = dynamic_cast<FrequencyScorer*>(scorer.get());
    if (freqScorer == nullptr) {
        throw CommandError("limita merge doar cu scorerul de frecventa (vezi :scorers)");
    }
    freqScorer->setLimit(limit);
}

std::vector<std::pair<int, double>> Ranker::rank(const InvIndex& idx,
                                                 const std::vector<std::string>& terms) const {
    if (idx.getDocCount() == 0) {
        return {};
    }

    int totalDocs = static_cast<int>(idx.getDocCount());
    double sumLen = 0.0;
    for (const auto& doc : idx.getDocuments()) {
        sumLen += idx.getDocTotalWords(doc.getId());
    }
    double avgLen = sumLen / totalDocs;

    std::map<int, double> scores;
    for (const auto& term : terms) {
        const std::vector<Entry>* entries = idx.findEntries(term);
        if (entries == nullptr) {
            continue;
        }
        int docsWithTerm = static_cast<int>(entries->size());
        for (const auto& entry : *entries) {
            ScoreContext ctx(totalDocs, docsWithTerm, idx.getDocTotalWords(entry.getDocID()), avgLen);
            scores[entry.getDocID()] += scorer->score(entry, ctx);
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

std::ostream& operator<<(std::ostream& os, const Ranker& ranker) {
    os << "Ranker cu " << *ranker.scorer;
    return os;
}