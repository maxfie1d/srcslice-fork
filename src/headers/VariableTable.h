#pragma once

#include "SliceProfile.hpp"
#include <functional>
#include <map>

typedef std::unordered_map<std::string, SliceProfile> VarMap;
typedef std::unordered_map<std::string, VarMap> FunctionVarMap;
typedef std::unordered_map<std::string, FunctionVarMap> FileFunctionVarMap;

class VariableTable {

private:
    FileFunctionVarMap _file_function_var_map;

public:
    VariableTable() = default;

    void forEach(std::function<void(SliceProfile)> f) {
        // ソートする
        std::map<std::string, FunctionVarMap> sorted_ffvMap
                (this->_file_function_var_map.begin(),
                 this->_file_function_var_map.end());
        for (auto &ffvmIt: sorted_ffvMap) {
            // ソートする
            std::map<std::string, VarMap> sorted_fvMap
                    (ffvmIt.second.begin(),
                     ffvmIt.second.end());
            for (auto fvmIt: sorted_fvMap) {
                // ソートする
                std::map<std::string, SliceProfile> sorted_vMap(
                        fvmIt.second.begin(),
                        fvmIt.second.end()
                );
                for (auto vmIt: sorted_vMap) {
                    f(vmIt.second);
//                    std::string row = varmap_pair_to_string(ffvmIt.first, fvmIt.first, &vmIt);
//                    ss << row << std::endl;
                }
            }
        }
    }

    const FileFunctionVarMap *getFileFunctionVarMap() const {
        return &this->_file_function_var_map;
    }

    FileFunctionVarMap::iterator addFile(std::string file_path) {
        auto pair = this->_file_function_var_map.insert(
                std::make_pair(file_path, FunctionVarMap())
        );
        return pair.first;
    }

    FileFunctionVarMap::iterator getFunctionVarMap(std::string file_path) {
        auto r = this->_file_function_var_map.find(file_path);
        if(r != this->_file_function_var_map.end()){
            return r;
        }else {
            nullptr;
        }
    }
};
