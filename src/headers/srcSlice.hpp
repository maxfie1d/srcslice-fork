#pragma once

#include <srcSliceHandler.hpp>
#include <Utility.hpp>
#include <stdio.h>

struct srcSlice {
/*This is a map of file, function/method, and variables. {file, {function, {SliceProfiles}}}*/
    SliceDictionary dictionary;

    srcSlice() {}

    srcSlice(const char *filename, const char *encoding);

    srcSlice(std::string buffer, const char *encoding);

    srcSlice(FILE *file, const char *encoding);

    srcSlice(int fd, const char *encoding);

    void ReadArchiveFile(std::string filename);

    /**
     * ソースファイルの数を返します
     * @return
     */
    unsigned long size() const { return dictionary.variableTable.getFileCount(); }

    bool SetContext(const std::string &file_path, const std::string &func_name, int linenumber) {
        auto function_var_map = dictionary.variableTable.getFunctionVarMap(file_path);
        auto var_map = dictionary.variableTable.getVarMap(file_path, func_name);
        if (function_var_map && var_map) {
            dictionary.currentContext.currentFile = function_var_map;
            dictionary.currentContext.currentVarMap = var_map;
            dictionary.currentContext.ln = linenumber;
            dictionary.currentContext.functionName = func_name;
            return true;
        }
        return false;
    }

    //Definition of find that assumes the user didn't give a context (They should just give a context, though, tbh).
    std::pair<bool, const SliceProfile *>
    Find(const std::string file_path, const std::string funcname, const std::string varname) {
        auto sp = dictionary.variableTable.getSliceProfile(file_path, funcname, varname);
        if (sp) {
            return std::make_pair(true, sp);
        } else {
            return std::make_pair<bool, const SliceProfile *>(false, nullptr);
        }
    }

    //Definition of find that assumes the user didn't give a context (They should just give a context, though, tbh).
    std::pair<bool, SliceProfile> Find(std::string funcname, std::string varname, int lineNumber) const {
        auto fvmIt = dictionary.currentContext.currentFile->find(funcname);
        if (fvmIt != dictionary.currentContext.currentFile->end()) {
            VarMap::const_iterator vtmIt = fvmIt->second.find(varname);
            if (vtmIt != fvmIt->second.end()) {
                return std::make_pair(true, vtmIt->second);
            }
        }
        return std::make_pair(false, SliceProfile());
    }

    //Definition of find that uses the context (so it doesn't need to take a function name as context)
    std::pair<bool, SliceProfile *> Find(const std::string &varname) {
        if (!dictionary.currentContext.IsSet()) {
            throw std::runtime_error("Context not set"); //for now, std exception
        } else {
            auto it = dictionary.currentContext.currentVarMap->find(varname);
            if (it != dictionary.currentContext.currentVarMap->end()) {
                return std::make_pair(true, &it->second);
            } else {
                // グローバルの方を探索する
                auto v_it = dictionary.variableTable.findGlobalVariableSliceProfileByName(varname);
                return v_it ? std::make_pair(true, v_it)
                            : std::make_pair<bool, SliceProfile *>(false, nullptr);
            }
        }
    }

    /*
     * 使われていないのでコメントアウト

    bool Insert(std::string flename, std::string funcname, const SliceProfile &np) {
        FileFunctionVarMap::iterator ffvmIt = dictionary.ffvMap.find(flename);
        if (ffvmIt != dictionary.ffvMap.end()) {
            FunctionVarMap::iterator fvmIt = ffvmIt->second.find(funcname);
            if (fvmIt != ffvmIt->second.end()) {
                VarMap::iterator vtmIt = fvmIt->second.find(np.variableName);
                if (vtmIt != fvmIt->second.end()) {
                    vtmIt->second = np;
                    return true;
                } else {
                    fvmIt->second.insert(std::make_pair(np.variableName, np));
                    return true;
                }
            }
        }
        return false;
    }

    bool Insert(std::string funcname, const SliceProfile &np) {
        if (dictionary.currentContext.currentFile != dictionary.ffvMap.end()) {
            FunctionVarMap::iterator fvmIt = dictionary.currentContext.currentFile->second.find(funcname);
            if (fvmIt != dictionary.currentContext.currentFile->second.end()) {
                VarMap::iterator vtmIt = fvmIt->second.find(np.variableName);
                if (vtmIt != fvmIt->second.end()) {
                    vtmIt->second = np;
                    return true;
                } else {
                    fvmIt->second.insert(std::make_pair(np.variableName, np));
                    return true;
                }
            }
        }
        return false;
    }

    bool Insert(const SliceProfile &np) {

        std::cout << "インサート" << std::endl;

        if (dictionary.currentContext.ln == -1) { //TODO: Make better
            throw std::runtime_error("Context not set"); //for now, std exception
        } else {
            auto it = dictionary.currentContext.currentFunc->second.find(np.variableName);
            if (it != dictionary.currentContext.currentFunc->second.end()) {
                it->second = np;
                return true;
            } else {
                dictionary.currentContext.currentFunc->second.insert(std::make_pair(np.variableName, np));
                return true;
            }
        }
        return false;
    }

    */

};
