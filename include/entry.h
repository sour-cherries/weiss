//
// Created by patri on 29.10.2026.
//

#ifndef OOP_ENTRY_H
#define OOP_ENTRY_H

#include <vector>
#include <iostream>

class Entry {
private:
    int docID;
    int freq;
    std::vector<int> pos;

    bool isValidIdx(int idx) const;

public:
    Entry(int docID, int firstPos);

    void addPos(int newPOS);

    int getDocID() const;
    int getFreq() const;
    const std::vector<int>& getPos() const;

    bool hasPos(int targetPos) const;
    int getFirstPos() const;
    int getLastPos() const;
    int getPosAt(int idx) const;
    int getSpan() const;
    int getMinGap() const;
    int getMaxGap() const;
    double getAvgGap() const;
    int countInRange(int startPos, int endPos) const;
    int getMinDistTo(const Entry& other) const;
    bool isFollowedBy(const Entry& other, int maxOffset) const;
    std::vector<int> getPositionsInWindow(int centerPos, int radius) const;
    double getRelativeDensity(int totalDocWords) const;

    std::string getProfile(const Entry& other, int totalDocWords) const;

    friend std::ostream& operator<<(std::ostream& os, const Entry& entry);
};

#endif //OOP_ENTRY_H