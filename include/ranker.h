//
// Created by patri on 26.11.2026.
//

#ifndef OOP_RANKER_H
#define OOP_RANKER_H

#include "index.h"
#include "scorer.h"

#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

class Ranker {
private:
    std::unique_ptr<Scorer> scorer;

public:
    explicit Ranker(const Scorer& s);
    Ranker(const Ranker& other);
    Ranker& operator=(Ranker other);
    ~Ranker();

    friend void swap(Ranker& first, Ranker& second);

    void setFrequencyLimit(int limit);
    std::vector<std::pair<int, double>> rank(const InvIndex& idx, const std::vector<std::string>& terms) const;

    friend std::ostream& operator<<(std::ostream& os, const Ranker& ranker);
};

#endif