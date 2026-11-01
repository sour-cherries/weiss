#undef NDEBUG
#include <cassert>
#include <string>
#include <vector>

#include "doc.h"
#include "entry.h"
#include "tokenizer.h"
#include "index.h"

int main() {
    Document d1(1, "doc1", "assets/doc1.txt", "programare c++");
    assert(d1.getId() == 1);
    assert(d1.getTitle() == "doc1");
    assert(d1.getfPath() == "assets/doc1.txt");
    assert(d1.getContent() == "programare c++");
    assert(d1.getLen() == 14);

    Document dCopie = d1;
    assert(dCopie.getId() == 1);
    assert(dCopie.getContent() == d1.getContent());

    Document dAtribuire(9, "dummy", "assets/dummy.txt", "altceva");
    dAtribuire = d1;
    assert(dAtribuire.getId() == 1);
    assert(dAtribuire.getTitle() == "doc1");

    Entry e(1, 0);
    assert(e.getDocID() == 1);
    assert(e.getFreq() == 1);
    assert(e.getPos().size() == 1);
    assert(e.getPos()[0] == 0);

    e.addPos(7);
    assert(e.getFreq() == 2);
    assert(e.getPos().size() == 2);
    assert(e.getPos()[1] == 7);

    Tokenizer tok(" \n\t.,;:!?-", true);
    assert(tok.getDelimiter() == " \n\t.,;:!?-");
    assert(tok.isLowenable());

    auto tokens = tok.tokenize("Programare, C++!");
    assert(tokens.size() == 2);
    assert(tokens[0].first == "programare" && tokens[0].second == 0);
    assert(tokens[1].first == "c++" && tokens[1].second == 1);

    auto tokensEmpty = tok.tokenize("   ,;!  ");
    assert(tokensEmpty.empty());

    InvIndex idx(tok);
    assert(idx.getDocCount() == 0);

    Document docA(1, "A", "assets/doc1.txt", "programare in c++");
    Document docB(2, "B", "assets/doc2.txt", "algoritm de cautare in c++");

    idx.addDoc(docA);
    idx.addDoc(docB);
    assert(idx.getDocCount() == 2);
    assert(idx.getDocuments().size() == 2);

    auto resComun = idx.searchExact("c++");
    assert(resComun.size() == 2);
    assert(resComun[0] == 1 && resComun[1] == 2);

    auto resAlgo = idx.searchExact("algoritm");
    assert(resAlgo.size() == 1);
    assert(resAlgo[0] == 2);

    auto resInexistent = idx.searchExact("java");
    assert(resInexistent.empty());

    auto sugg = idx.fuzzySugg("algortim", 2);
    assert(!sugg.empty());
    assert(sugg[0] == "algoritm");

    auto resAnd = idx.searchAND({"programare", "c++"});
    assert(resAnd.size() == 1);
    assert(resAnd[0] == 1);

    auto resOr = idx.searchOR({"programare", "algoritm"});
    assert(resOr.size() == 2);

    std::vector<std::string> used;
    auto resQuery = idx.searchQuery("programare and c++", used);
    assert(resQuery.size() == 1);
    assert(resQuery[0] == 1);


    assert(d1.countWords() >= 2);
    assert(d1.getExtension() == "txt");
    assert(e.hasPos(0));
    assert(e.getSpan() == 7);
    assert(tok.countTokens("a b c") == 3);
    assert(tok.isStopWord("the"));
    assert(idx.searchNOT("c++", "algoritm").size() == 1);
    assert(!idx.searchRanked("c++").empty());
    assert(idx.getVocabularySize() > 0);

    return 0;
}