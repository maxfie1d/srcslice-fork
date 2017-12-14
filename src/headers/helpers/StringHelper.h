#pragma once

#include <string>
#include <vector>

std::string join(const char delimiter, std::vector<std::string> source);

std::string computeSHA256Hash(std::string source);
