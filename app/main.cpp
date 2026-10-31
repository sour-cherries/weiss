#include "doc.h"
#include "entry.h"
#include "tokenizer.h"
#include "index.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
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

    int idCounter = 1;
    if (fs::exists("assets") && fs::is_directory("assets")) {
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
    }

    Entry e(1, 0);
    e.addPos(4);
    std::cout << e << "\n";
    std::cout << e.getDocID() << " " << e.getFreq() << " " << e.getPos().size() << "\n";

    std::cout << tok << "\n";
    std::cout << tok.getDelimiter() << " " << tok.isLowenable() << "\n";

    std::cout << idx.getDocCount() << " fisiere indexate complet din assets/\n";
    for (const auto& doc : idx.getDocuments()) {
        std::cout << doc.getTitle() << " (" << doc.getLen() << " caractere)\n";
    }
    std::cout << "\n";

    std::string input;
    std::cout << "ce cauti in fisiere: ";
    if (std::cin >> input) {
        auto gasite = idx.searchExact(input);
        std::cout << "termenul '" << input << "' a fost gasit in " << gasite.size() << " fisier(e):\n";

        for (int id : gasite) {
            for (const auto& doc : idx.getDocuments()) {
                if (doc.getId() == id) {
                    std::cout << " -> fisier: " << doc.getTitle()
                              << " | cale: " << doc.getfPath()
                              << " | lungime: " << doc.getLen() << " caractere\n";
                }
            }
        }

        if (gasite.empty()) {
            std::cout << "nu exista rezultate exacte. caut sugestii apropiate...\n";
            auto sugestii = idx.fuzzySugg(input, 2);
            for (const auto& s : sugestii) {
                std::cout << " -> ai vrut sa spui: " << s << "\n";
            }
        }
    }

    return 0;
}