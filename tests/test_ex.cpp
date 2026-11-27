#include <cassert>
#include <cmath>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#include "doc.h"
#include "entry.h"
#include "tokenizer.h"
#include "index.h"
#include "ranker.h"

int main() {
    Document d1(1, "Curs POO", "assets/doc1.txt", "Programare orientata pe obiecte in C++.\nLinia doi cu programare.");
    assert(d1.getId() == 1);
    assert(d1.getTitle() == "Curs POO");
    assert(d1.getPath() == "assets/doc1.txt");
    assert(!d1.getContent().empty());
    assert(!d1.getSnippet("programare", 10).empty());
    assert(d1.getLineOf("Linia") > 0);
    assert(d1.getLineOf("inexistent") < 0);
    assert(d1.countOccurrences("programare") >= 1);
    assert(d1.countOccurrences("inexistent") == 0);

    Document dCopy(d1);
    assert(dCopy.getId() == 1);
    assert(dCopy.getPath() == "assets/doc1.txt");

    Document dAssign(2, "Temp", "temp.txt", "alt continut");
    dAssign = d1;
    Document& dRef = dAssign;
    dAssign = dRef;
    assert(dAssign.getId() == 1);
    assert(dAssign.getTitle() == "Curs POO");

    {
        std::ofstream out("temp_test_doc.txt");
        out << "Continut citit din fisier.";
    }
    Document dFile = Document::fromFile(99, "temp_test_doc.txt");
    assert(dFile.getId() == 99);
    assert(!dFile.getContent().empty());
    std::remove("temp_test_doc.txt");

    std::ostringstream ossDoc;
    ossDoc << d1;
    assert(!ossDoc.str().empty());

    Entry e1(1, 2);
    e1.addPos(5);
    e1.addPos(12);
    assert(e1.getDocID() == 1);
    assert(e1.getFreq() == 3);
    assert(e1.getPos().size() == 3);
    assert(e1.getFirstPos() == 2);
    assert(e1.hasPos(5));
    assert(!e1.hasPos(99));

    Entry e2(1, 3);
    e2.addPos(7);
    auto phraseStarts = e1.getPhraseStarts(e2, 1);
    assert(phraseStarts.size() == 1 && phraseStarts[0] == 2);
    assert(e1.isFollowedBy(e2, 2));
    assert(e1.getMinDistTo(e2) == 1);
    assert(e1.getBestWindowStart(5) >= 0);
    assert(std::abs(e1.getRelativeDensity(30) - 0.1) < 1e-6);
    assert(e1.getBM25(1.5, 30, 25.0) > 0.0);

    std::ostringstream ossEntry;
    ossEntry << e1;
    assert(!ossEntry.str().empty());

    Tokenizer tok(" \n\t.,;:!?-", true, {"in", "pe", "cu", "si"});
    auto tokens = tok.tokenize("Programare in C++, si POO!");
    assert(tokens.size() == 5);
    assert(tokens[0].first == "programare" && tokens[0].second == 0);
    assert(tok.countTokens("Programare in C++, si POO!") == 5);
    assert(tok.normalizeWord("  C++! ") == "c++");
    assert(tok.isStopWord("in"));
    assert(!tok.isStopWord("programare"));

    auto filtered = tok.removeStopWords("Programare in C++, si POO!");
    assert(filtered.size() == 3);
    assert(Tokenizer::joinTokens(filtered, "-") == "programare-c++-poo");
    assert(!tok.analyzeSample("Programare in C++", ", ").empty());

    std::ostringstream ossTok;
    ossTok << tok;
    assert(!ossTok.str().empty());

    InvIndex idx(tok);
    Document d2(2, "Algoritmi", "assets/doc2.txt", "Algoritmi si structuri de date in C++.");
    idx.addDoc(d1);
    idx.addDoc(d2);
    assert(idx.getDocCount() == 2);
    assert(idx.getDocuments().size() == 2);

    assert(idx.searchExact("c++").size() == 2);
    auto exactAlg = idx.searchExact("algoritmi");
    assert(exactAlg.size() == 1 && exactAlg[0] == 2);

    assert(!idx.fuzzySugg("programre", 2).empty());
    assert(idx.bestTypoMatch("programre", 2) == "programare");

    auto autoComp = idx.autocomplete("prog");
    assert(autoComp.size() == 1 && autoComp[0] == "programare");

    std::string usedTerm;
    auto smartRes = idx.smartSearch("algoritm", 2, usedTerm);
    assert(usedTerm == "algoritmi" && smartRes.size() == 1 && smartRes[0] == 2);

    auto resAnd = idx.searchAND({"programare", "c++"});
    assert(resAnd.size() == 1 && resAnd[0] == 1);
    assert(idx.searchOR({"programare", "algoritmi"}).size() == 2);

    auto resNot = idx.searchNOT("c++", "algoritmi");
    assert(resNot.size() == 1 && resNot[0] == 1);

    std::vector<std::string> termsUsed;
    auto resQuery = idx.searchQuery("programare and c++", 2, termsUsed);
    assert(resQuery.size() == 1 && resQuery[0] == 1 && !termsUsed.empty());

    auto resPhrase = idx.searchPhrase("programare", "orientata", 1);
    assert(resPhrase.size() == 1 && resPhrase[0] == 1);

    auto resProx = idx.searchProximity("programare", "c++", 6);
    assert(resProx.size() == 1 && resProx[0] == 1);

    BM25Scorer bm25Test;
    Ranker rankerTest(bm25Test);
    auto resRanked = rankerTest.rank(idx, {"programare"});
    assert(!resRanked.empty() && resRanked[0].first == 1 && resRanked[0].second > 0.0);

    assert(!idx.locateTerm(1, "programare", 5, 2).empty());
    assert(!idx.describeDoc(1, {"programare", "c++"}, 5, 2).empty());

    assert(idx.getTermTotalFreq("programare") == 2);
    assert(idx.getTermDocFreq("c++") == 2);
    assert(!idx.getMostFrequentTerm().empty());
    assert(idx.getDocUniqueTerms(1) > 0);
    assert(idx.getDocTotalWords(1) > 0);
    assert(!idx.getTopTermsInDoc(1, 2).empty());
    assert(!idx.getIndexSummary().empty());

    std::ostringstream ossIdx;
    ossIdx << idx;
    assert(!ossIdx.str().empty());

    return 0;
}