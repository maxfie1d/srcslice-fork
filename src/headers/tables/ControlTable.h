#pragma once


#include <core/SliceProfile.hpp>

class ControlTable {
private:
    std::unordered_map<std::string, ControlData> _table;


public:
    ControlTable() = default;

    void add(ControlData controlData);

    unsigned long size() const;

    std::unordered_map<std::string, ControlData>::const_iterator begin() const;

    std::unordered_map<std::string, ControlData>::const_iterator end() const;
};
