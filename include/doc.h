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
    Document(int id, const std::string& title, const std::string& fPath, const std::string& content);

    Document(const Document& other);
    Document& operator=(const Document& other);
    ~Document();

     int getId() const;
     const std::string& getTitle() const;
     const std::string& getfPath() const;
     const std::string& getContent() const;

     std::size_t getLen() const;

    friend std::ostream& operator<<(std::ostream& os, const Document& doc);
};

#endif //OOP_DOC_H

