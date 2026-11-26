//
// Created by patri on 24.11.2026.
//

#ifndef OOP_SCORER_H
#define OOP_SCORER_H

#include "entry.h"

#include <iostream>
#include <memory>
#include <string>

struct ScoreContext {
    int totalDocs;
    int docsWithTerm;
    int docLen;
    double avgDocLen;

    ScoreContext(int totalDocs, int docsWithTerm, int docLen, double avgDocLen);
};

class Scorer {
private:
    std::string name;

    virtual double computeScore(const Entry& entry, const ScoreContext& ctx) const = 0;
    virtual void printDetails(std::ostream& os) const = 0;

public:
    explicit Scorer(const std::string& name);
    Scorer(const Scorer& other) = default;
    Scorer& operator=(const Scorer& other) = default;
    virtual ~Scorer();

    virtual std::unique_ptr<Scorer> clone() const = 0;

    double score(const Entry& entry, const ScoreContext& ctx) const;
    const std::string& getName() const;
    void print(std::ostream& os) const;

    friend std::ostream& operator<<(std::ostream& os, const Scorer& scorer);
};

class BM25Scorer : public Scorer {
private:
    double computeScore(const Entry& entry, const ScoreContext& ctx) const override;
    void printDetails(std::ostream& os) const override;

public:
    BM25Scorer();

    std::unique_ptr<Scorer> clone() const override;
};

class TfIdfScorer : public Scorer {
private:
    double computeScore(const Entry& entry, const ScoreContext& ctx) const override;
    void printDetails(std::ostream& os) const override;

public:
    TfIdfScorer();

    std::unique_ptr<Scorer> clone() const override;
};

class FrequencyScorer : public Scorer {
private:
    double computeScore(const Entry& entry, const ScoreContext& ctx) const override;
    void printDetails(std::ostream& os) const override;

public:
    FrequencyScorer();

    std::unique_ptr<Scorer> clone() const override;
};

#endif //OOP_SCORER_H