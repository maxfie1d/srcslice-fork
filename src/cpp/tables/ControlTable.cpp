#include <tables/ControlTable.h>

unsigned long ControlTable::size() const {
    return this->_table.size();
}

std::unordered_map<std::string, ControlData>::const_iterator ControlTable::begin() const {
    return std::begin(this->_table);
}

std::unordered_map<std::string, ControlData>::const_iterator ControlTable::end() const {
    return std::end(this->_table);
}

void ControlTable::add(ControlData controlData) {
    this->_table.insert(std::make_pair(controlData.id, controlData));
}

void ControlTable::dump() {
    for(auto p: this->_table){
        auto &cd = p.second;
        std::cout << cd.to_string() << std::endl;
    }
}
