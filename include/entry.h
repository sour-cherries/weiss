//
// Created by patri on 29.10.2026.
//

#ifndef OOP_ENTRY_H
#define OOP_ENTRY_H

#include <cstddef>
#include <iostream>
#include <vector>

class Entry {
private:
    int docID;
    int freq;
    std::vector<int> pos;

    static constexpr double K1 = 1.2;
    static constexpr double B = 0.75;

    std::size_t firstIndexNotBefore(int p) const;

public:
    Entry(int docID, int firstPos);

    void addPos(int newPos);

    int getDocID() const;
    int getFreq() const;
    const std::vector<int>& getPos() const;
    int getFirstPos() const;

    bool hasPos(int targetPos) const;
    std::vector<int> getPhraseStarts(const Entry& next, int offset) const;
    bool isFollowedBy(const Entry& other, int maxOffset) const;
    int getMinDistTo(const Entry& other) const;
    int getBestWindowStart(int windowSize) const;

    double getRelativeDensity(int totalDocWords) const;
    double getBM25(double idf, int docLen, double avgDocLen) const;

    friend std::ostream& operator<<(std::ostream& os, const Entry& entry);
};

#endif //OOP_ENTRY_H