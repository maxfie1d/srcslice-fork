#include "tables/FunctionTable.h"

FunctionData *FunctionTable::findById(std::string id) {
    return this->privateFind([&](FunctionData &fd) {
        return fd.computeId() == id;
    });
}

FunctionData *FunctionTable::findByName(std::string func_name) {
    return this->privateFind([&](FunctionData &fd) {
        return fd.functionName == func_name;
    });
}

FunctionData *FunctionTable::findByFilePathAndName(std::string file_path, std::string func_name) {
    return this->privateFind([&](FunctionData &fd) {
        return fd.fileName == file_path && fd.functionName == func_name;
    });
}

FunctionTable *FunctionTable::add(std::string function_name, FunctionData fd) {
    this->_fileFunctionTable.insert(std::make_pair(function_name, fd));
    return this;
}

unsigned long FunctionTable::size() const {
    return this->_fileFunctionTable.size();
}

FunctionData *FunctionTable::privateFind(std::function<bool(FunctionData &)> predicate) {
    for (auto &fd: this->_fileFunctionTable) {
        auto &second = fd.second;
        bool match = predicate(second);
        if (match) {
            return &fd.second;
        }
    }
    return nullptr;
}
