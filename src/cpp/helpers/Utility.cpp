#include "helpers/Utility.hpp"

std::string extractMemberName(std::string s) {
    auto pos = s.find('.');
    return pos == std::string::npos
           ? ""
           : s.substr(pos + 1, s.length() - 1);
}

std::string extractObjectName(std::string s) {
    auto pos = s.find('.');
    return pos == std::string::npos
           ? s
           : s.substr(0, pos);
}
