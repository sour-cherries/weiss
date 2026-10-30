//  
// Created by patri on 28.10.2026.
//

#include "doc.h"

Document::Document(int id, std::string title, std::string fPath, std::string content)
    : id(id), title(std::move(title)), fPath(std::move(fPath)), content(std::move(content)) {
}

Document::Document(const Document& other) = default;
Document& Document::operator=(const Document& other) = default;
Document::~Document() = default;

int Document::getId() const{
    return id;
}

const std::string& Document::getTitle() const{
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


