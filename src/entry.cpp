//
// Created by patri on 29.10.2026.
//

#include "entry.h"
#include <cstdlib>

Entry::Entry(int docID, int firstPos)
    : docID(docID), freq(1), pos{firstPos} {
}

void Entry::addPos(int newPos) {
    pos.push_back(newPos);
    freq++;
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

bool Entry::hasPos(int targetPos) const {
    for (int p : pos) {
        if (p == targetPos) {
            return true;
        }
    }
    return false;
}

int Entry::getFirstPos() const {
    if (pos.empty()) {
        return -1;
    }
    return pos.front();
}

int Entry::getLastPos() const {
    if (pos.empty()) {
        return -1;
    }
    return pos.back();
}

int Entry::getSpan() const {
    if (pos.size() < 2) {
        return 0;
    }
    return pos.back() - pos.front();
}

int Entry::countInRange(int startPos, int endPos) const {
    int count = 0;
    for (int p : pos) {
        if (p >= startPos && p <= endPos) {
            count++;
        }
    }
    return count;
}

int Entry::getMinDistTo(const Entry& other) const {
    if (pos.empty() || other.pos.empty()) {
        return -1;
    }
    int minDist = std::abs(pos[0] - other.pos[0]);
    for (int p1 : pos) {
        for (int p2 : other.pos) {
            int d = std::abs(p1 - p2);
            if (d < minDist) {
                minDist = d;
            }
        }
    }
    return minDist;
}

bool Entry::isFollowedBy(const Entry& other, int maxOffset) const {
    if (maxOffset <= 0) {
        return false;
    }
    for (int p1 : pos) {
        for (int p2 : other.pos) {
            int diff = p2 - p1;
            if (diff >= 1 && diff <= maxOffset) {
                return true;
            }
        }
    }
    return false;
}

std::ostream& operator<<(std::ostream& os, const Entry& entry) {
    os << "[DocID:" << entry.docID << " ~ Freq: " << entry.freq << "]";
    return os;
}