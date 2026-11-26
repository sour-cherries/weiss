//
// Created by patri on 26.11.2026.
//

#include "exceptions.h"

AppError::AppError(const std::string& message)
    : std::runtime_error(message) {
}

FileError::FileError(const std::string& message)
    : AppError("Eroare de fisier: " + message) {
}

CommandError::CommandError(const std::string& message)
    : AppError("Comanda gresita: " + message) {
}

DataError::DataError(const std::string& message)
    : AppError("Date invalide: " + message) {
}

IndexError::IndexError(const std::string& message)
    : AppError("Eroare la index: " + message) {
}