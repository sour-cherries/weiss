#include "doc.h"
#include "entry.h"
#include "tokenizer.h"
#include "index.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

std::string citesteFisier(const std::string& cale) {
    std::ifstream f(cale);
    if (!f.is_open()) {
        return "";
    }
    std::stringstream buf;
    buf << f.rdbuf();
    return buf.str();
}

int main() {
    Tokenizer tok(" \n\t.,;:!?-()[]{}\"'/\\", true);
    InvIndex idx(tok);

    if (fs::exists("assets") && fs::is_directory("assets")) {
        int idCounter = 1;
        for (const auto& entry : fs::directory_iterator("assets")) {
            if (entry.is_regular_file()) {
                std::string pathStr = entry.path().string();
                std::string content = citesteFisier(pathStr);
                if (!content.empty()) {
                    Document d(idCounter++, entry.path().filename().string(), pathStr, content);
                    idx.addDoc(d);
                }
            }
        }
    }

    if (idx.getDocCount() > 0) {
        Document dCopie = idx.getDocuments()[0];
        Document dAtribuire(99, "temp", "temp.txt", "text");
        dAtribuire = dCopie;

        std::cout << dCopie << "\n";
        std::cout << dAtribuire.getId() << " " << dAtribuire.getTitle() << " "
                  << dAtribuire.getfPath() << " " << dAtribuire.getContent().size()
                  << " " << dAtribuire.getLen() << "\n";

        std::cout << "linii: " << dAtribuire.countLines()
                  << " | cuvinte: " << dAtribuire.countWords()
                  << " | prop: " << dAtribuire.countSentences()
                  << " | paragrafe: " << dAtribuire.countParagraphs() << "\n";
        std::cout << "ext: " << dAtribuire.getExtension()
                  << " | folder: " << dAtribuire.getFolder()
                  << " | timp: " << dAtribuire.getReadingTimeMinutes() << " min\n";
        std::cout << "has 'the': " << dAtribuire.hasSubstring("the")
                  << " (" << dAtribuire.countSubstring("the") << ")"
                  << " | linii vec: " << dAtribuire.splitIntoLines().size()
                  << " | linia 0: " << dAtribuire.getLineAt(0).substr(0, 15)
                  << " | prev: " << dAtribuire.getPreview(25) << "\n";
    }

    Entry e(1, 0);
    e.addPos(4);
    e.addPos(9);
    Entry e2(1, 5);
    std::cout << e << " " << e2 << "\n";
    std::cout << e.getDocID() << " " << e.getFreq() << " " << e.getPos().size() << "\n";
    std::cout << "pos: " << e.hasPos(4) << " " << e.getFirstPos() << " " << e.getLastPos()
              << " " << e.getPosAt(1) << " span: " << e.getSpan() << "\n";
    std::cout << "gaps: " << e.getMinGap() << " " << e.getMaxGap() << " " << e.getAvgGap()
              << " range: " << e.countInRange(0, 6) << " dist: " << e.getMinDistTo(e2)
              << " follow: " << e.isFollowedBy(e2, 2)
              << " win: " << e.getPositionsInWindow(4, 2).size()
              << " dens: " << e.getRelativeDensity(100) << "\n";

    std::cout << tok << "\n";
    std::cout << tok.getDelimiter() << " " << tok.isLowenable() << "\n";

    std::string textProba = "the quick brown fox jumps over the lazy dog";
    auto faraStop = tok.removeStopWords(textProba);
    std::cout << "toks: " << tok.countTokens(textProba)
              << " | words: " << tok.extractWordsOnly(textProba).size()
              << " | unic: " << tok.extractUniqueWords(textProba).size()
              << " | min4: " << tok.filterByMinLength(textProba, 4).size() << "\n";
    std::cout << "stop('the'): " << tok.isStopWord("the")
              << " | noStop: " << faraStop.size()
              << " | join: " << tok.joinTokens(faraStop, "-")
              << " | norm: " << tok.normalizeWord(" Test! ")
              << " | strip: " << tok.stripDelimiters(textProba)
              << " | long: " << tok.findLongestToken(textProba)
              << " | avg: " << tok.getAverageTokenLength(textProba) << "\n";


    std::cout << tok.countTokens(textProba) << " " << tok.extractUniqueWords(textProba).size() << " "
              << tok.filterByMinLength(textProba, 4).size() << " " << tok.isStopWord("the") << " "
              << tok.removeStopWords(textProba).size() << " " << tok.findLongestToken(textProba) << "\n";

    std::cout << idx.getDocCount() << " fisiere indexate complet din assets/\n";
    for (const auto& doc : idx.getDocuments()) {
        std::cout << doc.getTitle() << " (" << doc.getLen() << " caractere)\n";
    }
    std::cout << "\n";

    std::cout << idx << "\n";

    auto exacte = idx.searchExact("security");
    std::cout << "cautare exacta demo: " << exacte.size() << " documente\n";

    auto sugestii = idx.fuzzySugg("secrity", 2);
    std::cout << "fuzzy demo: " << sugestii.size() << " sugestii\n";

    auto gasiteAnd = idx.searchAND({"security", "token"});
    std::cout << "cautare AND demo: " << gasiteAnd.size() << " documente\n";

    auto gasiteOr = idx.searchOR({"handshake", "certificate"});
    std::cout << "cautare OR demo: " << gasiteOr.size() << " documente\n\n";

    auto gasiteNot = idx.searchNOT("security", "blockchain");
    auto gasiteProx = idx.searchProximity("security", "protocol", 5);
    std::cout << "not: " << gasiteNot.size() << " prox: " << gasiteProx.size()
              << " freq: " << idx.getTermTotalFreq("security")
              << " top: " << idx.getMostFrequentTerm()
              << " doc1_unic: " << idx.getDocUniqueTerms(1)
              << " vocab: " << idx.getVocabularySize() << "\n";

    std::string input;
    std::cout << "ce cauti in fisiere: ";
    if (std::getline(std::cin, input)) {
        if (!input.empty() && input.back() == '\r') {
            input.pop_back();
        }

        std::vector<std::string> cuvinteFolosite;
        auto gasite = idx.searchQuery(input, cuvinteFolosite);

        std::cout << "rezultate pentru '" << input << "' (" << gasite.size() << " fisiere):\n";
        for (int id : gasite) {
            for (const auto& doc : idx.getDocuments()) {
                if (doc.getId() == id) {
                    std::cout << " -> [" << doc.getTitle() << "]\n";
                    for (const auto& w : cuvinteFolosite) {
                        std::string loc = idx.locateTerm(id, w);
                        if (!loc.empty()) {
                            std::cout << "    * " << w << ": " << loc << "\n";
                        }
                    }
                }
            }
        }

        if (!cuvinteFolosite.empty()) {
            std::string pref = cuvinteFolosite[0].size() >= 3 ? cuvinteFolosite[0].substr(0, 3) : cuvinteFolosite[0];
            auto sug = idx.autocomplete(pref);
            std::cout << "autocomplete '" << pref << "': ";
            for (const auto& s : sug) {
                std::cout << s << " ";
            }
            std::cout << "\n";
        }
    }

    return 0;
}