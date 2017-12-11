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

std::string extractArrayName(std::string s) {
    auto pos = s.find('[');
    return pos == std::string::npos
           ? s
           : s.substr(0, pos);
}

SliceProfile *varmapFind(std::string name, VarMap *p_varMap) {
    for (auto &a: *p_varMap) {
        if (a.first == name
            || extractArrayName(a.first) == name) {
            return &a.second;
        }
    }
    return nullptr;
}
