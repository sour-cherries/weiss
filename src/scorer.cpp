//
// Created by patri on 24.11.2026.
//

#include "scorer.h"

#include <cmath>

ScoreContext::ScoreContext(int totalDocs_, int docsWithTerm_, int docLen_, double avgDocLen_)
    : totalDocs(totalDocs_), docsWithTerm(docsWithTerm_), docLen(docLen_), avgDocLen(avgDocLen_) {
}

Scorer::Scorer(const std::string& name_)
    : name(name_) {
}

Scorer::~Scorer() {
}

double Scorer::score(const Entry& entry, const ScoreContext& ctx) const {
    if (ctx.totalDocs <= 0 || ctx.docsWithTerm <= 0 || ctx.docLen <= 0) {
        return 0.0;
    }
    return computeScore(entry, ctx);
}

const std::string& Scorer::getName() const {
    return name;
}

void Scorer::print(std::ostream& os) const {
    os << "Scorer [" << name << " ~ ";
    printDetails(os);
    os << "]";
}

std::ostream& operator<<(std::ostream& os, const Scorer& scorer) {
    scorer.print(os);
    return os;
}

BM25Scorer::BM25Scorer()
    : Scorer("bm25") {
}

double BM25Scorer::computeScore(const Entry& entry, const ScoreContext& ctx) const {
    double idf = std::log1p(static_cast<double>(ctx.totalDocs) / ctx.docsWithTerm);
    return entry.getBM25(idf, ctx.docLen, ctx.avgDocLen);
}

void BM25Scorer::printDetails(std::ostream& os) const {
    os << "tine cont de lungimea documentului si de raritatea termenului";
}

std::unique_ptr<Scorer> BM25Scorer::clone() const {
    return std::make_unique<BM25Scorer>(*this);
}

TfIdfScorer::TfIdfScorer()
    : Scorer("tfidf") {
}

double TfIdfScorer::computeScore(const Entry& entry, const ScoreContext& ctx) const {
    double idf = std::log1p(static_cast<double>(ctx.totalDocs) / ctx.docsWithTerm);
    double tf = entry.getRelativeDensity(ctx.docLen);
    return tf * idf;
}

void TfIdfScorer::printDetails(std::ostream& os) const {
    os << "densitatea cuvantului in document inmultita cu raritatea lui";
}

std::unique_ptr<Scorer> TfIdfScorer::clone() const {
    return std::make_unique<TfIdfScorer>(*this);
}

FrequencyScorer::FrequencyScorer()
    : Scorer("freq") {
}

double FrequencyScorer::computeScore(const Entry& entry, const ScoreContext&) const {
    return static_cast<double>(entry.getFreq());
}

void FrequencyScorer::printDetails(std::ostream& os) const {
    os << "doar de cate ori apare cuvantul in document";
}

std::unique_ptr<Scorer> FrequencyScorer::clone() const {
    return std::make_unique<FrequencyScorer>(*this);
}