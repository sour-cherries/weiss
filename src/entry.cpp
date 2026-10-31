//
// Created by patri on 29.10.2026.
//

#include "entry.h"

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

std::ostream& operator<<(std::ostream& os, const Entry& entry) {
    os << "[DocID:" << entry.docID << " ~ Freq: " << entry.freq << "]";
    return os;
}