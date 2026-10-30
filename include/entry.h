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

    void addPos(int pos);

    [[nodiscard]] int getDocID() const;
    [[nodiscard]] int getFreq() const;
    [[nodiscard]] const std::vector<int>& getPos() const;

    friend std::ostream& operator<<(std::ostream& os, const Entry& entry);
};

#endif //OOP_ENTRY_H
