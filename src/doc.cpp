//  
// Created by patri on 28.10.2026.
//

#include "doc.h"

#include "doc.h"

Document::Document(int id_, const std::string& title_, const std::string& fPath_, const std::string& content_)
    : id(id_), title(title_), fPath(fPath_), content(content_) {
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

bool Document::isSpace(char c) const {
    if (c == ' ' || c == '\n' || c == '\t' || c == '\r') {
        return true;
    }
    return false;
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
    if (content.empty()) {
        return 0;
    }
    int lines = 1;
    for (char c : content) {
        if (c == '\n') {
            lines++;
        }
    }
    return lines;
}

int Document::countWords() const {
    if (content.empty()) {
        return 0;
    }
    int words = 0;
    bool inWord = false;
    for (char c : content) {
        if (isSpace(c)) {
            if (inWord) {
                inWord = false;
            }
        } else {
            if (!inWord) {
                inWord = true;
                words++;
            }
        }
    }
    return words;
}

int Document::countSentences() const {
    if (content.empty()) {
        return 0;
    }
    int total = 0;
    for (char c : content) {
        if (c == '.' || c == '!' || c == '?') {
            total++;
        }
    }
    if (total == 0) {
        return 1;
    }
    return total;
}

int Document::countParagraphs() const {
    if (content.empty()) {
        return 0;
    }
    int total = 1;
    for (std::size_t i = 0; i + 1 < content.size(); ++i) {
        if (content[i] == '\n' && content[i + 1] == '\n') {
            total++;
        }
    }
    return total;
}

bool Document::hasSubstring(const std::string& sub) const {
    if (sub.empty()) {
        return false;
    }
    if (content.find(sub) != std::string::npos) {
        return true;
    }
    return false;
}

int Document::countSubstring(const std::string& sub) const {
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

std::string Document::getFolder() const {
    std::size_t slashPos = fPath.find_last_of("/\\");
    if (slashPos == std::string::npos) {
        return "";
    }
    return fPath.substr(0, slashPos);
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
    res += "...";
    return res;
}

std::vector<std::string> Document::splitIntoLines() const {
    std::vector<std::string> lines;
    std::string current;
    for (char c : content) {
        if (c == '\n') {
            lines.push_back(current);
            current.clear();
        } else if (c != '\r') {
            current.push_back(c);
        }
    }
    if (!current.empty()) {
        lines.push_back(current);
    }
    return lines;
}

std::string Document::getLineAt(int lineIndex) const {
    std::vector<std::string> all = splitIntoLines();
    if (lineIndex < 0) {
        return "";
    }
    if (static_cast<std::size_t>(lineIndex) >= all.size()) {
        return "";
    }
    return all[static_cast<std::size_t>(lineIndex)];
}

double Document::getReadingTimeMinutes() const {
    int w = countWords();
    if (w <= 0) {
        return 0.0;
    }
    return static_cast<double>(w) / 200.0;
}

std::ostream& operator<<(std::ostream& os, const Document& doc) {
    os << "Document ID: " << doc.id << " ~ Titlu: " << doc.title
       << " (" << doc.content.size() << " bytes)";
    return os;
}


