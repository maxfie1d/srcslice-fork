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

    FunctionData *privateFind(std::function<bool(FunctionData &)> predicate);

public:

    FunctionTable() = default;

    /**
     * 関数IDで関数を検索します
     * @param id
     * @return
     */
    FunctionData *findById(std::string id);

    /**
     * 関数名で関数を検索します
     * @param func_name
     * @return
     */
    FunctionData *findByName(std::string func_name);

    /**
     * ファイルパスと関数名で関数を検索します
     * @param file_path
     * @param func_name
     * @return
     */
    FunctionData *findByFilePathAndName(std::string file_path, std::string func_name);

    /**
     * 関数を関数テーブルに追加します
     * @param function_name
     * @param fd
     * @return
     */
    FunctionTable *add(std::string function_name, FunctionData fd);

    /**
     * 関数テーブルに登録されている関数の個数を返します
     * @return
     */
    unsigned long size() const;


    // beginとendイテレータを返すことで
    // for-each ができるようになる

    std::unordered_map<std::string, FunctionData>::const_iterator begin() const {
        return this->_fileFunctionTable.begin();
    }

    std::unordered_map<std::string, FunctionData>::const_iterator end() const {
        return std::end(this->_fileFunctionTable);
    }
};
