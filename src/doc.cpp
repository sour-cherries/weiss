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

int Document::countLines() const {
    if (content.empty()) return 0;

    int lines = 1;

    for (char c: content) {
        if (c == '\n') lines++;
    }

    return lines;
}

int Document::countWords() const {
    if (content.empty()) return 0;

    int words = 0;
    bool inWord = false;
    for (char c: content) {
        if (c == ' ' || c == '\n' || c == '\t' || c == '\r') inWord = false;
        else if (!inWord) {
            inWord = true;
            words++;
        }
    }
    return words;
}

bool Document::hasSubstring(const std::string& sub) const {
    if (sub.empty()) {
        return false;
    }
    return content.find(sub) != std::string::npos;
}

int Document::countSubstrings(const std::string& sub) const {
    if (sub.empty() || content.empty()) {
        return 0;
    }
    int count = 0;
    std::size_t pos = content.find(sub, 0);
    while (pos != std::string::npos) {
        count++;
        pos = content.find(sub, pos + sub.size());
    }
    return count;
}


std::string Document::getExtension() const {
    std::size_t dotPos = fPath.find_last_of('.');
    if (dotPos == std::string::npos) {
        return "";
    }
    return fPath.substr(dotPos + 1);
}

std::string Document::getPreview(std::size_t maxChars) const {
    if (content.size() <= maxChars) {
        return content;
    }
    std::string res = content.substr(0, maxChars);
    for (char& c : res) {
        if (c == '\n' || c == '\r') {
            c = ' ';
        }
    }
    return res + "...";
}

std::ostream& operator<<(std::ostream& os, const Document& doc) {
    os << "Document ID: " << doc.id << " ~ Titlu : " << doc.title;
    return os;
}


