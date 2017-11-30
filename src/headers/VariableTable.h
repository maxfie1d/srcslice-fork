#pragma once

#include "SliceProfile.hpp"
#include <functional>
#include <map>
#include <iostream>

typedef std::unordered_map<std::string, SliceProfile> VarMap;
typedef std::unordered_map<std::string, VarMap> FunctionVarMap;
typedef std::unordered_map<std::string, FunctionVarMap> FileFunctionVarMap;

class VariableTable {

private:
    /**
     * グローバル変数以外を格納する
     */
    FileFunctionVarMap _file_function_var_map;

    /**
     * グローバル変数を格納する
     */
    VarMap _global_var_map;

public:
    VariableTable() = default;

    void forEach(std::function<void(SliceProfile *)> f);

    unsigned long getFileCount() const {
        return this->_file_function_var_map.size();
    }

    const FileFunctionVarMap *getFileFunctionVarMap() const {
        return &this->_file_function_var_map;
    }

    FunctionVarMap *addFile(std::string file_path);

    VarMap *addFunction(const std::string file_path, const std::string func_name);

    FunctionVarMap *findFunctionVarMap(const std::string &file_path);

    VarMap *findVarMap(const std::string &file_path, const std::string &func_name);

    SliceProfile *
    findSliceProfile(const std::string file_path, const std::string func_name, const std::string var_name);

    SliceProfile *findGlobalVariableSliceProfileByName(const std::string &global_var_name);

    /**
     * グローバル変数を追加します
     * std::moveの挙動がよくわからなかったので、pairで渡すことにしている
     * @param var_name
     * @param sp
     */
    void addGlobalVariable(std::pair<std::string, SliceProfile> pair);

    VarMap *getRawGlobalVariableTable();

    /**
     * 変数テーブルの内容を標準出力にダンプします
     */
    void dump_to_stdout() const;
};
