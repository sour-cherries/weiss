#include "doc.h"
#include "entry.h"
#include "tokenizer.h"
#include "index.h"
#include "exceptions.h"
#include "ranker.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstddef>
#include <cstdlib>
#include <exception>
#include <filesystem>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include "scorer.h"
#include <memory>


namespace fs = std::filesystem;

const int NUMAR_MAXIM = 999999;
const int ASCII_MAXIM = 128;
const int DISTANTA_FRAZA = 1;
const std::size_t TERMENI_DEMO = 2;
const std::string CULOARE_START = "\033[1;33m";
const std::string CULOARE_STOP = "\033[0m";

struct Setari {
    int maxTypo = 2;
    std::size_t raza = 30;
    std::size_t maxPozitii = 5;
    double pragStopWords = 0.01;
    bool culori = true;
    std::size_t latimeBara = 40;
    std::size_t topTermeni = 5;
    std::size_t lungimeMostra = 100;
    int fereastraDemo = 10;
    int distantaDemo = 5;
};

std::vector<std::string> imparteCuvinte(const std::string& line) {
    std::vector<std::string> cuvinte;
    std::istringstream iss(line);
    std::string cuvant;
    while (iss >> cuvant) {
        cuvinte.push_back(cuvant);
    }
    return cuvinte;
}

bool citesteLinie(const std::string& prompt, std::string& line) {
    std::cout << prompt << std::flush;
    if (!std::getline(std::cin, line)) {
        return false;
    }
    if (!line.empty() && line.back() == '\r') {
        line.pop_back();
    }
    return true;
}

int citesteNumar(const std::string& text) {
    const std::string mesaj = "trebuie un numar intre 0 si " + std::to_string(NUMAR_MAXIM) + ", ai scris: " + text;
    if (text.empty() || text.size() > std::to_string(NUMAR_MAXIM).size()) {
        throw CommandError(mesaj);
    }
    for (char c : text) {
        if (std::isdigit(static_cast<unsigned char>(c)) == 0) {
            throw CommandError(mesaj);
        }
    }
    int numar = std::stoi(text);
    if (numar > NUMAR_MAXIM) {
        throw CommandError(mesaj);
    }
    return numar;
}

std::string construiesteDelimitatori() {
    std::string delim;
    for (int c = 1; c < ASCII_MAXIM; ++c) {
        if (std::ispunct(c) != 0) {
            delim.push_back(static_cast<char>(c));
        }
    }
    return delim;
}

std::vector<int> pozitiiTermen(const Tokenizer& tok, const std::string& text, const std::string& termen) {
    std::vector<int> pozitii;
    std::string curat = tok.normalizeWord(termen);
    for (const auto& p : tok.tokenize(text)) {
        if (p.first == curat) {
            pozitii.push_back(p.second);
        }
    }
    return pozitii;
}

Entry construiesteEntry(int docID, const std::vector<int>& pozitii) {
    if (pozitii.empty()) {
        throw DataError("un Entry are nevoie de cel putin o pozitie");
    }
    Entry entry(docID, pozitii.front());
    for (int p : pozitii) {
        entry.addPos(p);
    }
    return entry;
}

void afiseazaIds(const InvIndex& idx, const std::string& eticheta, const std::vector<int>& ids) {
    std::cout << eticheta << " (" << ids.size() << "):";
    for (int id : ids) {
        const Document* doc = idx.findDocById(id);
        if (doc != nullptr) {
            std::cout << " [" << doc->getTitle() << "]";
        }
    }
    std::cout << "\n";
}

void afiseazaCuvinte(const std::string& eticheta, const std::vector<std::string>& cuvinte) {
    std::cout << eticheta << " (" << cuvinte.size() << "): " << Tokenizer::joinTokens(cuvinte, ", ") << "\n";
}

std::vector<std::unique_ptr<Scorer>> construiesteScorere() {
    std::vector<std::unique_ptr<Scorer>> scorere;
    scorere.push_back(std::make_unique<BM25Scorer>());
    scorere.push_back(std::make_unique<TfIdfScorer>());
    scorere.push_back(std::make_unique<FrequencyScorer>());
    scorere.push_back(std::make_unique<PositionScorer>());
    return scorere;
}

const Scorer& gasesteScorer(const std::vector<std::unique_ptr<Scorer>>& scorere, const std::string& nume) {
    for (const auto& s : scorere) {
        if (s->hasName(nume)) {
            return *s;
        }
    }
    throw CommandError("nu exista scorerul " + nume + " (vezi :scorers)");
}

void afiseazaScorere(const std::vector<std::unique_ptr<Scorer>>& scorere) {
    for (const auto& s : scorere) {
        std::cout << *s << "\n";
    }
}

void afiseazaAjutor() {
    std::cout << "\nScrie orice cuvant sau mai multe pentru a cauta (greselile se corecteaza automat).\n"
              << "Poti folosi and / or / not:  algorithm and data   |   data not java\n\n"
              << "Comenzi:\n"
              << "  :help                  acest mesaj\n"
              << "  :stats                 statistici despre index si documente\n"
              << "  :complete <prefix>     autocomplete\n"
              << "  :phrase <w1> <w2>      cauta fraza (w1 urmat direct de w2)\n"
              << "  :near <w1> <w2> <n>    w1 si w2 la cel mult n cuvinte distanta\n"
              << "  :set typo|raza|pozitii <valoare>   schimba setarile\n"
              << "  :scorers               formulele de scor disponibile\n"
              << "  :set scorer <nume>     schimba formula de scor\n"
              << "  :compare <cuvinte>     compara clasamentul cu toate formulele\n"
              << "  :set limita <n>        maxim n aparitii numarate (doar la frecventa, 0 = fara limita)\n"
              << "  :history               cautarile de pana acum\n"
              << "  :again <nr>            repeta o cautare din istoric\n"
              << "  :index                 afiseaza tot indexul\n"
              << "  :demo                  demonstratie pentru Document / Entry / Tokenizer\n"
              << "  :quit                  iesire\n\n";
}

struct Aparitie {
    int linie;
    int coloana;
    int cuvant;
    std::size_t start;
    std::size_t sfarsit;
    std::size_t inceputLinie;
    std::size_t sfarsitLinie;
};

std::vector<Aparitie> gasesteAparitii(const Tokenizer& tok, const std::string& text,
                                      const std::string& delimitatori, const std::string& termen) {
    std::vector<Aparitie> rezultat;
    const std::string curat = tok.normalizeWord(termen);
    if (curat.empty()) {
        return rezultat;
    }

    int linie = 1;
    int indexCuvant = 0;
    std::size_t inceputLinie = 0;
    std::size_t startCuvant = 0;
    bool inCuvant = false;

    for (std::size_t i = 0; i <= text.size(); ++i) {
        bool separator = (i == text.size())
                         || std::isspace(static_cast<unsigned char>(text[i])) != 0
                         || delimitatori.find(text[i]) != std::string::npos;

        if (!separator && !inCuvant) {
            inCuvant = true;
            startCuvant = i;
        } else if (separator && inCuvant) {
            inCuvant = false;
            if (tok.normalizeWord(text.substr(startCuvant, i - startCuvant)) == curat) {
                std::size_t sfarsitLinie = text.find('\n', startCuvant);
                if (sfarsitLinie == std::string::npos) {
                    sfarsitLinie = text.size();
                }
                rezultat.push_back({linie, static_cast<int>(startCuvant - inceputLinie) + 1, indexCuvant,
                                    startCuvant, i, inceputLinie, sfarsitLinie});
            }
            indexCuvant++;
        }

        if (i < text.size() && text[i] == '\n') {
            linie++;
            inceputLinie = i + 1;
        }
    }
    return rezultat;
}

std::string construiesteBara(const std::vector<Aparitie>& aparitii, int totalCuvinte, std::size_t latime) {
    std::string bara(latime, '-');
    if (totalCuvinte <= 0) {
        return "[" + bara + "]";
    }
    for (const auto& a : aparitii) {
        std::size_t loc = static_cast<std::size_t>(a.cuvant) * latime / static_cast<std::size_t>(totalCuvinte);
        bara[std::min(loc, latime - 1)] = '#';
    }
    return "[" + bara + "]";
}

std::string construiesteContext(const std::string& text, const Aparitie& a, std::size_t raza, bool culori) {
    std::size_t start = (a.start > a.inceputLinie + raza) ? a.start - raza : a.inceputLinie;
    std::size_t end = std::min(a.sfarsitLinie, a.sfarsit + raza);

    std::string inainte = text.substr(start, a.start - start);
    std::string cuvant = text.substr(a.start, a.sfarsit - a.start);
    std::string dupa = text.substr(a.sfarsit, end - a.sfarsit);
    if (!dupa.empty() && dupa.back() == '\r') {
        dupa.pop_back();
    }

    std::string rezultat = (start > a.inceputLinie) ? "..." : "";
    rezultat += inainte;
    rezultat += culori ? CULOARE_START + cuvant + CULOARE_STOP : "[" + cuvant + "]";
    rezultat += dupa;
    if (end < a.sfarsitLinie) {
        rezultat += "...";
    }
    return rezultat;
}

void afiseazaRezultat(const InvIndex& idx, const Tokenizer& tok, const std::string& delimitatori,
                      int docID, double scor, const std::vector<std::string>& termeni, const Setari& s) {
    const Document* doc = idx.findDocById(docID);
    if (doc == nullptr) {
        return;
    }
    const int total = idx.getDocTotalWords(docID);
    std::cout << "[" << doc->getTitle() << "]  " << doc->getPath()
              << "  (scor " << scor << ", " << total << " cuvinte)\n";

    for (const auto& termen : termeni) {
        std::vector<Aparitie> aparitii = gasesteAparitii(tok, doc->getContent(), delimitatori, termen);
        if (aparitii.empty()) {
            continue;
        }
        std::cout << "  " << termen << " - " << aparitii.size() << " aparitii\n";
        std::cout << "    " << construiesteBara(aparitii, total, s.latimeBara) << "\n";

        std::size_t limita = std::min(aparitii.size(), s.maxPozitii);
        for (std::size_t i = 0; i < limita; ++i) {
            const Aparitie& a = aparitii[i];
            std::cout << "    linia " << a.linie << ", col " << a.coloana
                      << ", cuvantul " << a.cuvant + 1 << ":  "
                      << construiesteContext(doc->getContent(), a, s.raza, s.culori) << "\n";
        }
        if (aparitii.size() > limita) {
            std::cout << "    ... +" << aparitii.size() - limita << " altele (:set pozitii <n>)\n";
        }
    }
    std::cout << "\n";
}

void cautaSiAfiseaza(const InvIndex& idx, const Tokenizer& tok, const std::string& delimitatori,
                     const std::string& interogare, const Setari& s, const Ranker& ranker) {
    std::vector<std::string> folosite;
    std::vector<int> gasite = idx.searchQuery(interogare, s.maxTypo, folosite);

    if (folosite.empty()) {
        std::cout << "Niciun termen valid in interogare (poate sunt doar stop words).\n";
        return;
    }
    std::cout << "Termeni cautati: " << Tokenizer::joinTokens(folosite, ", ") << "\n";

    if (gasite.empty()) {
        std::cout << "Niciun rezultat.\n";
        std::vector<std::string> sugestii = idx.autocomplete(folosite.front());
        if (!sugestii.empty()) {
            afiseazaCuvinte("Poate vrei", sugestii);
        }
        return;
    }

    std::vector<std::pair<int, double>> ranking = ranker.rank(idx, folosite);
    std::cout << gasite.size() << " fisiere gasite:\n\n";
    for (const auto& scor : ranking) {
        if (std::find(gasite.begin(), gasite.end(), scor.first) != gasite.end()) {
            afiseazaRezultat(idx, tok, delimitatori, scor.first, scor.second, folosite, s);
        }
    }
}

void comparaScorere(const InvIndex& idx, const std::vector<std::unique_ptr<Scorer>>& scorere,
                    const std::string& interogare, int maxTypo) {
    std::vector<std::string> folosite;
    std::vector<int> gasite = idx.searchQuery(interogare, maxTypo, folosite);
    if (gasite.empty()) {
        std::cout << "Niciun rezultat.\n";
        return;
    }

    for (const auto& s : scorere) {
        Ranker ranker(*s);
        std::cout << ranker << "\n";
        int loc = 1;
        for (const auto& rezultat : ranker.rank(idx, folosite)) {
            if (std::find(gasite.begin(), gasite.end(), rezultat.first) == gasite.end()) {
                continue;
            }
            const Document* doc = idx.findDocById(rezultat.first);
            if (doc != nullptr) {
                std::cout << "  " << loc << ". " << doc->getTitle() << " (scor " << rezultat.second << ")\n";
                loc++;
            }
        }
    }
}

void afiseazaStatistici(const InvIndex& idx, const Setari& s) {
    std::cout << idx.getIndexSummary() << "\n";
    std::cout << "Scoruri calculate pana acum: " << Scorer::getComputedScores() << "\n";
    for (const auto& doc : idx.getDocuments()) {
        std::cout << "doc " << doc.getId() << " [" << doc.getTitle() << "]: "
                  << idx.getDocUniqueTerms(doc.getId()) << " termeni unici, "
                  << idx.getDocTotalWords(doc.getId()) << " cuvinte, top: "
                  << Tokenizer::joinTokens(idx.getTopTermsInDoc(doc.getId(), s.topTermeni), ", ") << "\n";
    }
}

void ruleazaDemo(const InvIndex& idx, const Tokenizer& tok, const Setari& s,
                 const std::vector<std::unique_ptr<Scorer>>& scorere, const Ranker& ranker) {
    const Document& primul = idx.getDocuments().front();
    Document copie(primul);
    Document atribuit = idx.getDocuments().back();
    atribuit = copie;
    std::cout << "~~ Document ~~\n" << copie << "\n" << atribuit << "\n";
    std::cout << "calea: " << copie.getPath() << " | id: " << copie.getId()
              << " | titlu: " << copie.getTitle() << " | lungime: " << copie.getContent().size() << "\n";

    std::vector<std::string> top = idx.getTopTermsInDoc(primul.getId(), TERMENI_DEMO);
    if (top.empty()) {
        std::cout << "Documentul nu are termeni indexati.\n";
        return;
    }
    const std::string& t1 = top.front();
    const std::string& t2 = top.back();
    std::cout << "'" << t1 << "': " << copie.countOccurrences(t1) << " aparitii, linia "
              << copie.getLineOf(t1) << ", snippet: " << copie.getSnippet(t1, s.raza) << "\n\n";

    std::cout << "~~ Tokenizer ~~\n" << tok << "\n"
              << tok.analyzeSample(copie.getContent().substr(0, s.lungimeMostra), " ") << "\n"
              << "'" << t1 << "' stop word? " << tok.isStopWord(t1) << "\n\n";

    Entry a = construiesteEntry(primul.getId(), pozitiiTermen(tok, primul.getContent(), t1));
    Entry b = construiesteEntry(primul.getId(), pozitiiTermen(tok, primul.getContent(), t2));
    int total = idx.getDocTotalWords(primul.getId());
    double avgLen = static_cast<double>(total);
    double idf = std::log1p(static_cast<double>(idx.getDocCount()) / std::max(1, idx.getTermDocFreq(t1)));

    std::cout << "~~ Entry ('" << t1 << "' si '" << t2 << "') ~~\n" << a << "\n" << b << "\n";
    std::cout << "docID=" << a.getDocID() << " freq=" << a.getFreq() << " prima poz=" << a.getFirstPos()
              << " nr pozitii=" << a.getPos().size() << "\n";
    std::cout << "A are pozitia primei pozitii din B? " << a.hasPos(b.getFirstPos()) << "\n";
    std::cout << "inceputuri de fraza A->B: " << a.getPhraseStarts(b, DISTANTA_FRAZA).size() << "\n";
    std::cout << "A urmat de B in " << s.distantaDemo << " cuvinte? " << a.isFollowedBy(b, s.distantaDemo) << "\n";
    std::cout << "distanta minima A-B: " << a.getMinDistTo(b) << "\n";
    std::cout << "cea mai densa fereastra (" << s.fereastraDemo << ") A incepe la: "
              << a.getBestWindowStart(s.fereastraDemo) << "\n";
    std::cout << "densitate relativa A: " << a.getRelativeDensity(total)
              << " | BM25 A: " << a.getBM25(idf, total, avgLen) << "\n";
    ScoreContext ctx(static_cast<int>(idx.getDocCount()), idx.getTermDocFreq(t1), total, avgLen);
    std::cout << "\n~~ Toate scorerele ~~\n";
    for (const auto& scorer : scorere) {
        std::cout << *scorer << "\n";
        std::cout << "  scor: " << scorer->score(a, ctx) << "\n";
    }

    std::cout << "\n~~ Ranker ~~\n";
    Ranker copieRanker(ranker);
    Ranker altRanker(*scorere.back());
    std::cout << altRanker << "\n";
    altRanker = copieRanker;
    std::cout << altRanker << "\n";
}

void afiseazaIstoric(const std::vector<std::string>& istoric) {
    if (istoric.empty()) {
        std::cout << "Nu ai cautat nimic inca.\n";
        return;
    }
    for (std::size_t i = 0; i < istoric.size(); ++i) {
        std::cout << i + 1 << ". " << istoric[i] << "\n";
    }
}

void proceseazaComanda(const std::string& linie, const InvIndex& idx, const Tokenizer& tok, Setari& s,
                       const std::vector<std::unique_ptr<Scorer>>& scorere, Ranker& ranker,
                       const std::string& delimitatori, const std::vector<std::string>& istoric) {
    std::vector<std::string> arg = imparteCuvinte(linie);
    const std::string& cmd = arg.front();

    if (cmd == ":help") {
        afiseazaAjutor();
    } else if (cmd == ":stats") {
        afiseazaStatistici(idx, s);
    } else if (cmd == ":index") {
        std::cout << idx << "\n";
    } else if (cmd == ":demo") {
        ruleazaDemo(idx, tok, s, scorere, ranker);
    } else if (cmd == ":history") {
        afiseazaIstoric(istoric);
    } else if (cmd == ":again" && arg.size() >= 2) {
        std::size_t numar = static_cast<std::size_t>(citesteNumar(arg[1]));
        if (numar == 0 || numar > istoric.size()) {
            throw CommandError("nu exista cautarea " + arg[1] + " (vezi :history)");
        }
        cautaSiAfiseaza(idx, tok, delimitatori, istoric[numar - 1], s, ranker);
    } else if (cmd == ":scorers") {
        afiseazaScorere(scorere);
        std::cout << "Folosit acum: " << ranker << "\n";
    } else if (cmd == ":compare" && arg.size() >= 2) {
        std::vector<std::string> cuvinte(arg.begin() + 1, arg.end());
        comparaScorere(idx, scorere, Tokenizer::joinTokens(cuvinte, " "), s.maxTypo);
    } else if (cmd == ":complete" && arg.size() >= 2) {
        afiseazaCuvinte("autocomplete '" + arg[1] + "'", idx.autocomplete(arg[1]));
    } else if (cmd == ":phrase" && arg.size() >= 3) {
        afiseazaIds(idx, "fraza '" + arg[1] + " " + arg[2] + "'", idx.searchPhrase(arg[1], arg[2], DISTANTA_FRAZA));
    } else if (cmd == ":near" && arg.size() >= 4) {
        afiseazaIds(idx, "proximitate", idx.searchProximity(arg[1], arg[2], citesteNumar(arg[3])));
    } else if (cmd == ":set" && arg.size() >= 3 && arg[1] == "scorer") {
        ranker = Ranker(gasesteScorer(scorere, arg[2]));
        std::cout << "ok: " << ranker << "\n";
    } else if (cmd == ":set" && arg.size() >= 3 && arg[1] == "limita") {
        ranker.setFrequencyLimit(citesteNumar(arg[2]));
        std::cout << "ok: " << ranker << "\n";
    } else if (cmd == ":set" && arg.size() >= 3) {
        int valoare = citesteNumar(arg[2]);
        if (arg[1] == "typo") {
            s.maxTypo = valoare;
        } else if (arg[1] == "raza") {
            s.raza = static_cast<std::size_t>(valoare);
        } else if (arg[1] == "pozitii") {
            s.maxPozitii = static_cast<std::size_t>(valoare);
        } else if (arg[1] == "culori") {
            s.culori = (valoare != 0);
        } else {
            throw CommandError("setare necunoscuta: " + arg[1]);
        }
        std::cout << "ok: typo=" << s.maxTypo << " raza=" << s.raza << " pozitii=" << s.maxPozitii << "\n";
    } else {
        throw CommandError("comanda necunoscuta sau argumente lipsa");
    }
}

std::vector<std::string> detecteazaStopWords(const std::vector<std::string>& cai,
                                             const std::string& delimitatori, double prag) {
    Tokenizer faraStop(delimitatori, true, {});
    std::map<std::string, int> frecvente;
    int total = 0;

    for (const auto& cale : cai) {
        try {
            Document doc = Document::fromFile(0, cale);
            for (const auto& p : faraStop.tokenize(doc.getContent())) {
                frecvente[p.first]++;
                total++;
            }
        } catch (const FileError&) {
        }
    }

    std::vector<std::string> rezultat;
    for (const auto& item : frecvente) {
        if (total > 0 && static_cast<double>(item.second) / total >= prag) {
            rezultat.push_back(item.first);
        }
    }
    return rezultat;
}

std::vector<std::string> citesteStopWordsDinFisier(const std::string& cale) {
    std::ifstream fisier(cale);
    if (!fisier.is_open()) {
        throw FileError("nu pot deschide fisierul cu stop words " + cale);
    }
    std::stringstream buffer;
    buffer << fisier.rdbuf();
    return imparteCuvinte(buffer.str());
}

int main(int argc, char* argv[]) {
    try {
        Setari setari;

        std::string folder;
        if (argc > 1) {
            folder = argv[1];
            argv[1] = nullptr;
        } else if (!citesteLinie("folder cu documente: ", folder)) {
            return EXIT_FAILURE;
        }

        if (!fs::is_directory(folder)) {
            throw FileError("nu exista folderul " + folder);
        }

        std::vector<std::string> cai;
        for (const auto& intrare : fs::directory_iterator(folder)) {
            if (intrare.is_regular_file()) {
                cai.push_back(intrare.path().string());
            }
        }
        std::sort(cai.begin(), cai.end());

        const std::string delimitatori = construiesteDelimitatori();
        std::vector<std::string> stopWords;
        if (argc > 2) {
            stopWords = citesteStopWordsDinFisier(argv[2]);
            argv[2] = nullptr;
        } else {
            stopWords = detecteazaStopWords(cai, delimitatori, setari.pragStopWords);
        }
        afiseazaCuvinte("stop words", stopWords);

        Tokenizer tok(delimitatori, true, stopWords);

        InvIndex idx(tok);
        const std::vector<std::unique_ptr<Scorer>> scorere = construiesteScorere();
        Ranker ranker(*scorere.front());
        for (const auto& cale : cai) {
            try {
                idx.addDoc(Document::fromFile(static_cast<int>(idx.getDocCount()), cale));
            } catch (const FileError& e) {
                std::cerr << "Document sarit: " << e.what() << "\n";
            }
        }
        if (idx.getDocCount() == 0) {
            throw IndexError("nu exista niciun document de indexat in " + folder);
        }

        std::cout << "\nAm indexat " << idx.getDocCount() << " documente.\n" << idx.getIndexSummary() << "\n";
        afiseazaAjutor();

        std::vector<std::string> istoric;
        std::string linie;
        while (citesteLinie("cauta> ", linie)) {
            if (imparteCuvinte(linie).empty()) {
                continue;
            }
            if (linie == ":quit" || linie == ":q") {
                break;
            }
            try {
                if (linie.front() == ':') {
                    proceseazaComanda(linie, idx, tok, setari, scorere, ranker, delimitatori, istoric);
                } else {
                    cautaSiAfiseaza(idx, tok, delimitatori, linie, setari, ranker);
                    istoric.push_back(linie);
                }
            } catch (const CommandError& e) {
                std::cout << e.what() << "\nScrie :help pentru comenzi.\n";
            } catch (const AppError& e) {
                std::cout << e.what() << "\n";
            } catch (const std::exception& e) {
                std::cout << "Eroare: " << e.what() << "\n";
            }
            std::cout << "\n";
        }
    } catch (const AppError& e) {
        std::cerr << e.what() << "\n";
        return EXIT_FAILURE;
    } catch (const std::exception& e) {
        std::cerr << "Eroare: " << e.what() << "\n";
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}