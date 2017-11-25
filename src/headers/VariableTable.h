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
    FileFunctionVarMap _file_function_var_map;

public:
    VariableTable() = default;

    void forEach(std::function<void(SliceProfile)> f);

    unsigned long getFileCount() const {
        return this->_file_function_var_map.size();
    }

    const FileFunctionVarMap *getFileFunctionVarMap() const {
        return &this->_file_function_var_map;
    }

    FunctionVarMap *addFile(std::string file_path);

    VarMap *addFunction(const std::string file_path, const std::string func_name);

    FunctionVarMap *getFunctionVarMap(const std::string &file_path);

    VarMap *getVarMap(const std::string &file_path, const std::string &func_name);

    SliceProfile *
    getSliceProfile(const std::string file_path, const std::string func_name, const std::string var_name);

    /**
     * 変数テーブルの内容を標準出力にダンプします
     */
    void dump_to_stdout() const;
};
