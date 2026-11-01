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
        Document dAtribuire(99, "temp.txt", "assets/temp.txt", "text");
        dAtribuire = dCopie;
        std::cout << dCopie << "\n" << dAtribuire.getStats() << "\n\n";
    }

    Entry e1(1, 0);
    e1.addPos(4);
    Entry e2(1, 1);
    std::cout << e1 << "\n" << e1.getProfile(e2, 50) << "\n\n";

    std::cout << tok << "\n" << tok.analyzeSample("the quick brown fox") << "\n\n";

    std::cout << idx << "\n" << idx.runDiagnostics() << "\n\n";

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
                    std::cout << " -> [" << doc.getTitle() << "] (" << doc.getfPath() << ")\n";
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