//
// Created by patri on 28.10.2026.
//

#ifndef OOP_DOC_H
#define OOP_DOC_H

#include<string>
#include<iostream>

class Document {
private:
    int id;
    std::string title;
    std::string fPath;
    std::string content;

public:
    Document(int id, std::string title,  std::string fPath, std::string content);

    Document(const Document& other);
    Document& operator=(const Document& other);
    ~Document();

    [[nodiscard]] int getId() const;
    [[nodiscard]] const std::string& getTitle() const;
    [[nodiscard]] const std::string& getfPath() const;
    [[nodiscard]] const std::string& getContent() const;

    [[nodiscard]] std::size_t getLen() const;

    friend std::ostream& operator<<(std::ostream& os, const Document& doc);
};

#endif //OOP_DOC_H

