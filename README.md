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

Un motor de căutare în documente text, scris în C++ pentru proiectul de OOP.
Îi dai un folder cu fișiere, el le indexează, iar tu cauți cuvinte, fraze sau combinații cu `and` / `or` / `not`. Greșelile de scriere se corectează automat.

## Build

Ai nevoie de:

- CMake 3.28 sau mai nou
- un compilator cu suport C++23 (GCC 13+, Clang 17+ sau MSVC 2022)

Nu are dependențe externe, folosește doar biblioteca standard.

```bash
git clone <link-repo>
cd <nume-repo>
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

Rezultă biblioteca `liboop.a` (clasele din `src/`) și executabilul `oop_main` (din `app/main.cpp`).

Opțiuni utile:

```bash
cmake -B build -DCMAKE_BUILD_TYPE=Debug     # pentru depanare
cmake -B build -DUSE_ASAN=ON                # AddressSanitizer
cmake --build build --parallel              # compilare mai rapidă
```

Pe Windows (Visual Studio), executabilul e în `build\Release\oop_main.exe`.

## Rulare

```bash
./build/oop_main <folder_cu_documente> [fisier_stop_words]
```

- Dacă nu dai folderul ca argument, programul te întreabă de el.
- Dacă nu dai fișier de stop words, acestea se detectează automat (cuvintele care apar foarte des).

Exemple de date de intrare sunt în `tastatura.txt`:

```bash
./build/oop_main assets < tastatura.txt
```

## Exemple de căutare

```
cauta> algorithm
cauta> algorithm and data
cauta> data not java
cauta> algoritm            <- greseala, se corecteaza singur
```

## Comenzi

| Comandă | Ce face |
|---|---|
| `:help` | afișează ajutorul |
| `:stats` | statistici despre index și documente |
| `:complete <prefix>` | autocomplete |
| `:phrase <w1> <w2>` | caută o frază (w1 urmat direct de w2) |
| `:near <w1> <w2> <n>` | w1 și w2 la cel mult n cuvinte distanță |
| `:set typo\|raza\|pozitii\|culori <valoare>` | schimbă setările |
| `:index` | afișează tot indexul |
| `:demo` | demonstrație pentru clasele proiectului |
| `:quit` | ieșire |


## Resurse folosite

- [Introduction to Information Retrieval](https://nlp.stanford.edu/IR-book/) (Manning, Raghavan, Schütze): inverted index, tf-idf, scoring
- [learncpp.com](https://www.learncpp.com/): moștenire, funcții virtuale, smart pointers, excepții
- [cppreference.com](https://en.cppreference.com/): documentația STL
- [CMake Documentation](https://cmake.org/cmake/help/latest/): configurarea build-ului
- [Refactoring Guru](https://refactoring.guru/design-patterns): design patterns
- [C++ Core Guidelines](https://github.com/isocpp/CppCoreGuidelines): clase și ierarhii
- [Levenshtein distance](https://en.wikipedia.org/wiki/Levenshtein_distance): corectarea greșelilor de scriere
- [Okapi BM25](https://en.wikipedia.org/wiki/Okapi_BM25): formula de scor folosită la ranking

## Licență

The project is licensed under [AGPLv3](https://github.com/sour-cherries/weiss/blob/main/LICENSE).

The [template repository](https://github.com/mcmarius/oop-template) itself is licensed under [Unlicense](https://github.com/sour-cherries/weiss/blob/main/LICENSE.template).

