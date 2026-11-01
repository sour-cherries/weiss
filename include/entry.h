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

public:
    Entry(int docID, int firstPos);

    void addPos(int newPos);

    int getDocID() const;
    int getFreq() const;
    const std::vector<int>& getPos() const;

    bool hasPos(int targetPos) const;
    int getFirstPos() const;
    int getLastPos() const;
    int getSpan() const;
    int countInRange(int startPos, int endPos) const;
    int getMinDistTo(const Entry& other) const;
    bool isFollowedBy(const Entry& other, int maxOffset) const;

    friend std::ostream& operator<<(std::ostream& os, const Entry& entry);
};

#endif //OOP_ENTRY_H
