//
// Created by patri on 29.10.2026.
//

#include "entry.h"
#include <cstdlib>

Entry::Entry(int docID_, int firstPos)
    : docID(docID_), freq(1), pos{firstPos} {
}

bool Entry::isValidIdx(int idx) const {
    if (idx < 0) {
        return false;
    }
    if (static_cast<std::size_t>(idx) >= pos.size()) {
        return false;
    }
    return true;
}

void Entry::addPos(int newPOS) {
    if (newPOS < 0) {
        return;
    }
    pos.push_back(newPOS);
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

int Entry::getPosAt(int idx) const {
    if (!isValidIdx(idx)) {
        return -1;
    }
    return pos[static_cast<std::size_t>(idx)];
}

int Entry::getSpan() const {
    if (pos.size() < 2) {
        return 0;
    }
    return pos.back() - pos.front();
}

int Entry::getMinGap() const {
    if (pos.size() < 2) {
        return 0;
    }
    int minG = pos[1] - pos[0];
    for (std::size_t i = 2; i < pos.size(); ++i) {
        int diff = pos[i] - pos[i - 1];
        if (diff < minG) {
            minG = diff;
        }
    }
    return minG;
}

int Entry::getMaxGap() const {
    if (pos.size() < 2) {
        return 0;
    }
    int maxG = pos[1] - pos[0];
    for (std::size_t i = 2; i < pos.size(); ++i) {
        int diff = pos[i] - pos[i - 1];
        if (diff > maxG) {
            maxG = diff;
        }
    }
    return maxG;
}

double Entry::getAvgGap() const {
    if (pos.size() < 2) {
        return 0.0;
    }
    int sum = 0;
    for (std::size_t i = 1; i < pos.size(); ++i) {
        sum += (pos[i] - pos[i - 1]);
    }
    return static_cast<double>(sum) / static_cast<double>(pos.size() - 1);
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

std::vector<int> Entry::getPositionsInWindow(int centerPos, int radius) const {
    std::vector<int> res;
    if (radius < 0) {
        return res;
    }
    int left = centerPos - radius;
    int right = centerPos + radius;
    for (int p : pos) {
        if (p >= left && p <= right) {
            res.push_back(p);
        }
    }
    return res;
}

double Entry::getRelativeDensity(int totalDocWords) const {
    if (totalDocWords <= 0) {
        return 0.0;
    }
    return static_cast<double>(freq) / static_cast<double>(totalDocWords);
}

std::ostream& operator<<(std::ostream& os, const Entry& entry) {
    os << "[DocID:" << entry.docID << " ~ Freq: " << entry.freq << "]";
    return os;
}