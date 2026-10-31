//  
// Created by patri on 28.10.2026.
//

#include "doc.h"

Document::Document(int id, const std::string& title, const std::string& fPath, const std::string& content)
    : id(id), title(title), fPath(fPath), content(content) {
}

Document::Document(const Document& other)
    : id(other.id), title(other.title), fPath(other.fPath), content(other.content) {
}

Document& Document::operator=(const Document& other) {
    if (this != &other) {
        id = other.id;
        title = other.title;
        fPath = other.fPath;
        content = other.content;
    }
    return *this;
}

Document::~Document() {
}

int Document::getId() const {
    return id;
}

const std::string& Document::getTitle() const {
    return title;
}

const std::string& Document::getfPath() const {
    return fPath;
}

const std::string& Document::getContent() const {
    return content;
}

std::size_t Document::getLen() const {
    return content.size();
}

std::ostream& operator<<(std::ostream& os, const Document& doc) {
    os << "Document ID: " << doc.id << " ~ Titlu : " << doc.title;
    return os;
}


