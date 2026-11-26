//  
// Created by patri on 28.10.2026.
//

#include "doc.h"
#include "exceptions.h"

#include <algorithm>
#include <cctype>
#include <cstddef>
#include <fstream>
#include <sstream>

Document::Document(int id_, const std::string& title_, const std::string& path_, const std::string& content_)
    : id(id_), title(title_), path(path_), content(content_) {
}

Document::Document(const Document& other)
    : id(other.id), title(other.title), path(other.path), content(other.content) {
}

Document& Document::operator=(const Document& other) {
    if (this != &other) {
        id = other.id;
        title = other.title;
        path = other.path;
        content = other.content;
    }
    return *this;
}

Document::~Document() {
}

std::string Document::toLower(const std::string& s) {
    std::string result = s;
    for (char& c : result) {
        c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
    }
    return result;
}

std::string Document::flatten(const std::string& s) {
    std::string result = s;
    for (char& c : result) {
        if (c == '\n' || c == '\r' || c == '\t') {
            c = ' ';
        }
    }
    return result;
}

std::size_t Document::findIgnoreCase(const std::string& term) const {
    if (term.empty()) {
        return std::string::npos;
    }
    return toLower(content).find(toLower(term));
}

Document Document::fromFile(int id, const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        throw FileError("nu pot deschide fisierul " + path);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string title = path;
    std::size_t slash = title.find_last_of("/\\");
    if (slash != std::string::npos) {
        title = title.substr(slash + 1);
    }
    std::size_t dot = title.find_last_of('.');
    if (dot != std::string::npos && dot > 0) {
        title.resize(dot);
    }

    return Document(id, title, path, buffer.str());
}

int Document::getId() const {
    return id;
}

const std::string& Document::getTitle() const {
    return title;
}

const std::string& Document::getPath() const {
    return path;
}

const std::string& Document::getContent() const {
    return content;
}

std::string Document::getSnippet(const std::string& term, std::size_t radius) const {
    std::size_t pos = findIgnoreCase(term);
    if (pos == std::string::npos) {
        return "";
    }

    std::size_t matchEnd = pos + term.size();
    std::size_t start = 0;
    if (pos > radius) {
        start = pos - radius;
    }
    std::size_t end = std::min(content.size(), matchEnd + radius);

    std::string result;
    if (start > 0) {
        result += "...";
    }
    result += flatten(content.substr(start, pos - start));
    result += "[" + flatten(content.substr(pos, term.size())) + "]";
    result += flatten(content.substr(matchEnd, end - matchEnd));
    if (end < content.size()) {
        result += "...";
    }
    return result;
}

int Document::getLineOf(const std::string& term) const {
    std::size_t pos = findIgnoreCase(term);
    if (pos == std::string::npos) {
        return -1;
    }
    auto endIt = content.begin() + static_cast<std::ptrdiff_t>(pos);
    int newlines = static_cast<int>(std::count(content.begin(), endIt, '\n'));
    return newlines + 1;
}

int Document::countOccurrences(const std::string& term) const {
    if (term.empty()) {
        return 0;
    }
    std::string text = toLower(content);
    std::string wanted = toLower(term);

    int count = 0;
    std::size_t pos = text.find(wanted);
    while (pos != std::string::npos) {
        count++;
        pos = text.find(wanted, pos + wanted.size());
    }
    return count;
}

std::ostream& operator<<(std::ostream& os, const Document& doc) {
    os << "Document ID: " << doc.id << " ~ Titlu: " << doc.title
       << " (" << doc.content.size() << " bytes)";
    return os;
}