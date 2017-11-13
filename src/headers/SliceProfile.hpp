/**
 * @file SliceProfile.hpp
 *
 * @copyright Copyright (C) 2013-2014 SDML (www.srcML.org)
 *
 * The srcML Toolkit is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * The srcML Toolkit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with the srcML Toolkit; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#pragma once

#include <string>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <set>
#include "srcSliceTypes.h"

class SliceProfile;

typedef std::unordered_map<std::string, SliceProfile> VarMap;
typedef std::unordered_map<std::string, VarMap> FunctionVarMap;
typedef std::unordered_map<std::string, FunctionVarMap> FileFunctionVarMap;


struct NameLineNumberPairHash {
public:
    template<typename T, typename U>
    std::size_t operator()(const std::pair<T, U> &x) const {
        return std::hash<T>()(x.first);
    }
};

/**
 * 関数のデータを格納する構造体
 */
struct FunctionData {
    /**
     * 返り値の型
     */
    std::string returnType;

    /**
     * 関数名
     */
    std::string functionName;

    /**
     * 関数の定義されているソースファイルのパス
     */
    std::string fileName;

    /**
     * vectorのsizeは引数の数であり、内容は引数の型である
     * size of vector is # of arguments. String is type of argument.
     */
    std::vector<std::string> params;

    /**
     * 関数が定義されている範囲
     */
    ProgramRange declareRange;

    /**
     * コンストラクタ
     */
    FunctionData() {
    }

    /**
     * 返り値の型と関数名と宣言範囲をクリアします
     */
    void clear() {
        returnType.clear();
        functionName.clear();
        declareRange.clear();
    }
};

struct ClassProfile {
    std::string className;
    std::unordered_set<std::string> memberVariables;
    //std::unordered_set<FunctionData, FunctionArgtypeArgnumHash> memberFunctions; //need to handle overloads. Can't be string.
};

class SliceProfile {
public:
    SliceProfile() : index(0), visited(false), potentialAlias(false), dereferenced(false), isGlobal(false) {}

    SliceProfile(unsigned int idx,
                 std::string fle,
                 std::string fcn,
                 unsigned int sline,
                 std::string name,
                 bool alias = false,
                 bool global = false)
            : index(idx), file(fle), function(fcn), potentialAlias(alias), variableName(name), isGlobal(global) {
        dereferenced = false;
        visited = false;
    }

    unsigned int index;
    std::unordered_set<std::string>::iterator lastInsertedAlias;
    bool potentialAlias;
    bool dereferenced;
    bool isGlobal;
    bool visited;

    /**
     * その変数が存在するソースファイルのパス
     */
    std::string file;

    /**
     * その変数が存在する関数の名前
     */
    std::string function;

    /**
     * 変数名
     */
    std::string variableName;

    /**
     * 変数の型
     */
    std::string variableType;

    /**
     * なにこれ
     */
    std::unordered_set<std::string> memberVariables;

    /**
     * Deprecated
     */
    std::unordered_set<unsigned int> slines;

    /**
     * defされる行番号の集合
     */
    std::set<unsigned int> def;

    /**
     * useされる行番号の集合
     */
    std::set<unsigned int> use;

    /**
     * cfuncs{}
     */
    std::unordered_set<std::pair<std::string, unsigned int>, NameLineNumberPairHash> cfunctions;

    /**
     * dvars{}
     */
    std::unordered_set<std::string> dvars;

    /**
     * pointers{}
     */
    std::unordered_set<std::string> aliases;
};
