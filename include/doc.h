//
// Created by patri on 28.10.2026.
//

#ifndef OOP_DOC_H
#define OOP_DOC_H

#include <string>
#include <vector>
#include <iostream>

class Document {
private:
    int id;
    std::string title;
    std::string fPath;
    std::string content;

    bool isSpace(char c) const;

public:
    Document(int id, const std::string& title, const std::string& fPath, const std::string& content);
    Document(const Document& other);
    Document& operator=(const Document& other);
    ~Document();

    int getId() const;
    const std::string& getTitle() const;
    const std::string& getfPath() const;
    const std::string& getContent() const;
    std::size_t getLen() const;

    int countLines() const;
    int countWords() const;
    int countSentences() const;
    int countParagraphs() const;
    bool hasSubstring(const std::string& sub) const;
    int countSubstring(const std::string& sub) const;
    std::string getExtension() const;
    std::string getFolder() const;
    std::string getPreview(std::size_t maxChars) const;
    std::string getLineAt(int lineIndex) const;
    std::vector<std::string> splitIntoLines() const;
    double getReadingTimeMinutes() const;

    friend std::ostream& operator<<(std::ostream& os, const Document& doc);
};

#endif //OOP_DOC_H

