```
__        __ _____ ___ ____  ____
\ \      / /| ____|_ _/ ___|/ ___|
 \ \ /\ / / |  _|  | |\___ \\___ \
  \ V  V /  | |___ | | ___) |___) |
   \_/\_/   |_____|___|____/|____/
    find your words with Weiss  


        .-"""-.
       /  o o  \
      |    O    |  ,---.
      |         | / ?_? \    <- "psst... its on the line 42, column 7"
      '~\/\/\/~'  \_____/\
                          \
```

Weiss e un motor de căutare în documente text, scris în C++ pentru cursul de OOP. Îi dai un folder cu fișiere, le indexează, iar tu cauți cuvinte, fraze sau combinații cu `and` / `or` / `not`. Greșelile de scriere se corectează automat, iar rezultatele sunt ordonate după relevanță.

Structura proiectului e din template (de asta are CMake, AGENTS.md, GitHub Actions și licența AGPL).

Încă nu are interfață în terminal, doar linie de comandă. Indexul se face de la zero la fiecare pornire, iar `and` / `or` / `not` nu merg cu paranteze.

## Build și rulare

Ai nevoie de CMake 3.28+ și un compilator cu C++23.

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug --parallel 6
./build/oop_main assets < tastatura.txt
```

Fără `< tastatura.txt` poți scrie căutările tu. Pentru teste: `ctest --test-dir build -C Debug --output-on-failure`.

## Exemple

```
cauta> algorithm and data
cauta> data not java
cauta> algoritm            <- greseala, se corecteaza singur
```

## Comenzi

- `:help` lista de comenzi
- `:stats` statistici despre index și documente
- `:index` afișează tot indexul
- `:demo` demonstrație pentru clasele proiectului
- `:complete <prefix>` completează un prefix cu cuvinte din index
- `:phrase <w1> <w2>` w1 urmat direct de w2
- `:near <w1> <w2> <n>` w1 și w2 la cel mult n cuvinte distanță
- `:scorers` formulele de scor (`bm25`, `tfidf`, `freq`, `pozitie`, `densitate`)
- `:set scorer <nume>` schimbă formula de scor
- `:compare <cuvinte>` compară clasamentul făcut de toate formulele
- `:set typo|raza|pozitii|culori|limita <valoare>` schimbă setările
- `:history` căutările de până acum
- `:again <nr>` repetă o căutare din istoric
- `:quit` ieșire

## Resurse

- [Introduction to Information Retrieval](https://nlp.stanford.edu/IR-book/): indexul inversat și tf-idf
- [Okapi BM25](https://en.wikipedia.org/wiki/Okapi_BM25): formula de scor din `Entry::getBM25`
- [Levenshtein distance](https://en.wikipedia.org/wiki/Levenshtein_distance): corectarea greșelilor, în `src/index.cpp`
- [learncpp.com](https://www.learncpp.com/)
- [cppreference.com](https://en.cppreference.com/)
- [C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines)
- [Refactoring Guru](https://refactoring.guru/design-patterns)
- [CMake Documentation](https://cmake.org/cmake/help/latest/)

## Licență

The project is licensed under [AGPLv3](https://github.com/sour-cherries/weiss/blob/main/LICENSE).

The [template repository](https://github.com/mcmarius/oop-template) itself is licensed under [Unlicense](https://github.com/sour-cherries/weiss/blob/main/LICENSE.template)./sour-cherries/weiss/blob/main/LICENSE.template).