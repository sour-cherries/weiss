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

        std::cout << "linii: " << dAtribuire.countLines() << " cuvinte: " << dAtribuire.countWords()
                  << " ext: " << dAtribuire.getExtension() << " has 'the': " << dAtribuire.hasSubstring("the")
                  << " (" << dAtribuire.countSubstrings("the") << ") preview: " << dAtribuire.getPreview(30) << "\n";
    }

    Entry e(1, 0);
    e.addPos(4);
    Entry e2(1, 5);
    std::cout << e.hasPos(4) << " " << e.getFirstPos() << " " << e.getLastPos() << " "
              << e.getSpan() << " " << e.countInRange(0, 5) << " "
              << e.getMinDistTo(e2) << " " << e.isFollowedBy(e2, 2) << "\n";
    std::cout << e << "\n";
    std::cout << e.getDocID() << " " << e.getFreq() << " " << e.getPos().size() << "\n";

    std::cout << tok << "\n";
    std::cout << tok.getDelimiter() << " " << tok.isLowenable() << "\n";

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