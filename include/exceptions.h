//
// Created by patri on 26.11.2026.
//

#ifndef OOP_EXCEPTIONS_H
#define OOP_EXCEPTIONS_H

#include <stdexcept>
#include <string>

class AppError : public std::runtime_error {
public:
    explicit AppError(const std::string& message);
};

class FileError : public AppError {
public:
    explicit FileError(const std::string& message);
};

class CommandError : public AppError {
public:
    explicit CommandError(const std::string& message);
};

class DataError : public AppError {
public:
    explicit DataError(const std::string& message);
};

class IndexError : public AppError {
public:
    explicit IndexError(const std::string& message);
};

#endif //OOP_EXCEPTIONS_H