#pragma once

#include "SliceProfile.hpp"
#include <functional>
#include <iostream>


/**
 * 関数テーブルを表すクラスです
 */
class FunctionTable {
private:
    std::unordered_map<std::string, FunctionData> _fileFunctionTable;

    FunctionData *privateFind(std::function<bool(FunctionData &)> predicate) {
        for (auto& fd: this->_fileFunctionTable) {
            std::cout << fd.second.functionName << std::endl;
            auto &second = fd.second;
            bool match = predicate(second);
            if (match) {
                return &fd.second;
            }
        }
        return nullptr;
    }

public:

    FunctionTable() = default;

    /**
     * 関数IDで関数を検索します
     * @param id
     * @return
     */
    FunctionData *findById(std::string id) {
        return this->privateFind([&](FunctionData &fd) {
            return fd.computeId() == id;
        });
    }

    /**
     * 関数名で関数を検索します
     * @param func_name
     * @return
     */
    FunctionData *findByName(std::string func_name) {
        return this->privateFind([&](FunctionData &fd) {
            return fd.functionName == func_name;
        });
    }

    /**
     * 関数を関数テーブルに追加します
     * @param function_name
     * @param fd
     * @return
     */
    FunctionTable *add(std::string function_name, FunctionData fd) {
        this->_fileFunctionTable.insert(std::make_pair(function_name, fd));
        return this;
    }

    /**
     * 関数テーブルに登録されている関数の個数を返します
     * @return
     */
    unsigned long size() const {
        return this->_fileFunctionTable.size();
    }
};
