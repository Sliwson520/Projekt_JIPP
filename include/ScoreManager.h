#ifndef SCORE_MANAGER_H
#define SCORE_MANAGER_H

#pragma once
#include "API.h"
#include <string>
#include <stdexcept>

class ENGINE_API FileException : public std::runtime_error {
public:
    FileException(const std::string& msg) : std::runtime_error(msg) {}
};

class ENGINE_API ScoreManager {
public:
    static void saveHighScore(int score, const std::string& filename);
    static int loadHighScore(const std::string& filename);
};

#endif