//
// Created by patri on 28.10.2026.
//

#ifndef OOP_DOC_H
#define OOP_DOC_H

#include <string>
#include <iostream>

class Document {
private:
    int id;
    std::string title;
    std::string path;
    std::string content;

    static std::string toLower(const std::string& s);
    static std::string flatten(const std::string& s);
    std::size_t findIgnoreCase(const std::string& term) const;

public:
    Document(int id, const std::string& title, const std::string& path, const std::string& content);
    Document(const Document& other);
    Document& operator=(const Document& other);
    ~Document();

    static Document fromFile(int id, const std::string& path);

    int getId() const;
    const std::string& getTitle() const;
    const std::string& getPath() const;
    const std::string& getContent() const;

    std::string getSnippet(const std::string& term, std::size_t radius) const;
    int getLineOf(const std::string& term) const;
    int countOccurrences(const std::string& term) const;

    friend std::ostream& operator<<(std::ostream& os, const Document& doc);
};

#endif //OOP_DOC_H