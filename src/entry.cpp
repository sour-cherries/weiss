//
// Created by patri on 29.10.2026.
//

#include "entry.h"
#include "exceptions.h"

#include <algorithm>
#include <cstdlib>
#include <string>

Entry::Entry(int docID_, int firstPos)
    : docID(docID_), freq(1), pos{firstPos} {
    if (firstPos < 0) {
        throw DataError("pozitia unui cuvant nu poate fi negativa");
    }
}

std::size_t Entry::firstIndexNotBefore(int p) const {
    auto it = std::lower_bound(pos.begin(), pos.end(), p);
    return static_cast<std::size_t>(it - pos.begin());
}

void Entry::addPos(int newPos) {
    if (newPos < 0) {
        return;
    }
    std::size_t idx = firstIndexNotBefore(newPos);
    if (idx < pos.size() && pos[idx] == newPos) {
        return;
    }
    pos.insert(pos.begin() + static_cast<std::ptrdiff_t>(idx), newPos);
    freq = static_cast<int>(pos.size());
}

int Entry::getDocID() const {
    return docID;
}

int Entry::getFreq() const {
    return freq;
}

const std::vector<int>& Entry::getPos() const {
    return pos;
}

int Entry::getFirstPos() const {
    if (pos.empty()) {
        return -1;
    }
    return pos.front();
}

bool Entry::hasPos(int targetPos) const {
    std::size_t idx = firstIndexNotBefore(targetPos);
    return idx < pos.size() && pos[idx] == targetPos;
}

std::vector<int> Entry::getPhraseStarts(const Entry& next, int offset) const {
    std::vector<int> starts;
    if (offset <= 0) {
        return starts;
    }
    for (int p : pos) {
        if (next.hasPos(p + offset)) {
            starts.push_back(p);
        }
    }
    return starts;
}

bool Entry::isFollowedBy(const Entry& other, int maxOffset) const {
    if (maxOffset <= 0) {
        return false;
    }
    for (int p : pos) {
        std::size_t idx = other.firstIndexNotBefore(p + 1);
        if (idx < other.pos.size() && other.pos[idx] - p <= maxOffset) {
            return true;
        }
    }
    return false;
}

int Entry::getMinDistTo(const Entry& other) const {
    if (pos.empty() || other.pos.empty()) {
        return -1;
    }
    std::size_t i = 0;
    std::size_t j = 0;
    int best = std::abs(pos[0] - other.pos[0]);
    while (i < pos.size() && j < other.pos.size()) {
        int d = std::abs(pos[i] - other.pos[j]);
        if (d < best) {
            best = d;
        }
        if (pos[i] < other.pos[j]) {
            i++;
        } else {
            j++;
        }
    }
    return best;
}

int Entry::getBestWindowStart(int windowSize) const {
    if (pos.empty()) {
        return -1;
    }
    if (windowSize <= 0) {
        return pos.front();
    }

    std::size_t bestIdx = 0;
    std::size_t bestCount = 0;
    std::size_t right = 0;

    for (std::size_t left = 0; left < pos.size(); ++left) {
        if (right < left) {
            right = left;
        }
        while (right < pos.size() && pos[right] < pos[left] + windowSize) {
            right++;
        }
        std::size_t count = right - left;
        if (count > bestCount) {
            bestCount = count;
            bestIdx = left;
        }
    }
    return pos[bestIdx];
}

double Entry::getRelativeDensity(int totalDocWords) const {
    if (totalDocWords <= 0) {
        return 0.0;
    }
    return static_cast<double>(freq) / static_cast<double>(totalDocWords);
}


double Entry::getBM25(double idf, int docLen, double avgDocLen) const {
    if (docLen <= 0 || avgDocLen <= 0.0) {
        return 0.0;
    }
    double tf = static_cast<double>(freq);
    double norm = 1.0 - B + B * (static_cast<double>(docLen) / avgDocLen);
    return idf * (tf * (K1 + 1.0)) / (tf + K1 * norm);
}

std::ostream& operator<<(std::ostream& os, const Entry& entry) {
    os << "[DocID:" << entry.docID << " ~ Freq: " << entry.freq << " ~ Pos:";
    std::size_t limit = std::min(entry.pos.size(), Entry::MAX_POZ_AFISATE);
    for (std::size_t i = 0; i < limit; ++i) {
        os << " " << entry.pos[i];
    }
    if (entry.pos.size() > limit) {
        os << " ...";
    }
    os << "]";
    return os;
}