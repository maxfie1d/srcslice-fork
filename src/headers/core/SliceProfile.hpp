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
#include "types/srcSliceTypes.h"
#include "types/ProgramPoint.hpp"

class SliceProfile;

struct NameLineNumberPairHash {
public:
    template<typename T, typename U>
    std::size_t operator()(const std::pair<T, U> &x) const {
        return std::hash<T>()(x.first);
    }
};

struct DefUseData {
    ProgramPoint programPoint;

    /**
     * 構造体ならばメンバの名前を入れる
     */
    std::string member_name;

    /**
     * 関数から伝播してきた場合、その関数のIDを保持する
     */
    std::string derived_from_func_id;

    DefUseData() = default;

    DefUseData(ProgramPoint programPoint) {
        this->programPoint = programPoint;
    }

    DefUseData(ProgramPoint programPoint, std::string member_name) {
        this->programPoint = programPoint;
        this->member_name = member_name;
    }

    bool operator<(const DefUseData &other) const {
        if (this->programPoint == other.programPoint) {
            return this->member_name < other.member_name;
        } else {
            return this->programPoint < other.programPoint;
        }
    }

    bool operator==(const DefUseData &other) const {
        this->programPoint == other.programPoint && this->member_name == other.member_name;
    }

    std::string to_string() const {
        auto member_name_as_str = this->member_name.empty()
                                  ? ""
                                  : "(" + this->member_name + ")";
        auto derived_from_func_id_as_str = this->derived_from_func_id.empty()
                                           ? ""
                                           : "<from_" + this->derived_from_func_id + ">";
        return this->programPoint.to_string() + member_name_as_str + derived_from_func_id_as_str;
    }
};

struct FileData {
    std::string file_path;
    unsigned int line_number;

    FileData() = default;

    FileData(std::string file_path, unsigned int line_number) {
        this->file_path = file_path;
        this->line_number = line_number;
    }

    std::string to_string() const {
        return this->file_path + ":" + std::to_string(this->line_number);
    }

    bool empty() const {
        return this->file_path.empty();
    }
};


// 本来DvarにはVariableDataのポインタかSliceProfileのポインタを
// 持たせたいが、SrcSliceクラスに辞書を検索するメソッドがあったりで
// コード修正が必要なため、応急処置でDvarData構造体を作った
struct DvarData {
    std::string variableName;
    std::string variableId;

    DvarData(std::string variableName, std::string variableId) {
        this->variableName = variableName;
        this->variableId = variableId;
    }

    std::string to_string() {
        // NOTE: どの変数という情報は保持しているが、
        // それをどこでという情報がない気がする。
        // というわけで行番号+関数IDの情報も追加していいかも
        return this->variableName + "(" + this->variableId + ")";
    }

    // < 演算子をオーバーライドすれば
    // 任意の型のSet<>を作ることができる
    bool operator<(const DvarData &other) const {
//        return false;
        if (this->variableId == other.variableId) {
            return this->variableName < other.variableName;
        } else {
            return this->variableId < other.variableId;
        }
    }
};

struct ArgIndexAndMemberName {
    unsigned int index;
    std::string member_name;

    ArgIndexAndMemberName() = default;

    ArgIndexAndMemberName(unsigned int index, std::string member_name = "") {
        this->index = index;
        this->member_name = member_name;
    }

    std::string to_string() const {
        if (this->member_name.empty()) {
            return std::to_string(this->index);
        } else {
            return std::to_string(this->index) + "(" + this->member_name + ")";
        }
    }
};

/**
 * cfuncsのデータを保持する構造体
 */
struct CFuncData {
    /**
     * その変数を引数として呼び出している関数
     */
    std::string calledFunctionName;

    /**
     * 関数ID
     */
    std::string calledFunctionId;

    /**
     * 何番目の引数として呼び出されたか(one-based)
     */
    ArgIndexAndMemberName argIndenx;

    /**
     * 関数呼び出しが起きた場所
     */
    ProgramPoint location;

    CFuncData(
            std::string calledFunctionName,
            std::string functionId,
            ArgIndexAndMemberName argIndenx,
            ProgramPoint location
    ) {
        this->calledFunctionName = calledFunctionName;
        this->calledFunctionId = functionId;
        this->argIndenx = argIndenx;
        this->location = location;
    }

    std::string to_string() const;

    bool operator<(const CFuncData &other) const;
};

/**
 * if 文の範囲を表す構造体です
 */
struct ControlRange {
    unsigned int startLine = 0;
    unsigned int elseLine = 0;
    unsigned int endLine = 0;

    ControlRange() = default;

    ControlRange(unsigned int startLine,
                 unsigned int endLine) {
        this->startLine = startLine;
        this->endLine = endLine;
    }

    ControlRange(unsigned int startLine,
                 unsigned int elseLine,
                 unsigned int endLine) {
        this->startLine = startLine;
        this->elseLine = elseLine;
        this->endLine = endLine;
    }

    std::string to_string() const {
        if (this->elseLine == 0) {
            return std::to_string(this->startLine) + ".." + std::to_string(this->endLine);
        } else {
            return std::to_string(this->startLine) + ".." + std::to_string(this->elseLine) + ".." +
                   std::to_string(this->endLine);
        }
    }
};

/**
 * 制御表のレコードを表すクラスです
 */
struct ControlData {
    std::string id;
    ControlRange controlRange;
    std::set<std::string> vars;

    ControlData(
            std::string id,
            ControlRange controlRange,
            std::set<std::string> vars
    ) {
        this->id = id;
        this->controlRange = controlRange;
        this->vars = vars;
    }

    bool operator<(const ControlData &other) const {
        return this->id < other.id;
    }

    bool operator==(const ControlData &other) const {
        return this->id == other.id;
    }
};


/**
 * 関数のデータを格納する構造体
 */
struct FunctionData {
private:
    /**
     * 関数ID
     */
    std::string id;

public:
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
    FunctionData() = default;

    /**
     * 返り値の型と関数名と宣言範囲をクリアします
     */
    void clear();

    /**
     * 関数IDを計算します。計算済みの場合はその値を返します
     * @return
     */
    std::string computeId();
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
                 FileData fle,
                 std::string fcn,
                 unsigned int sline,
                 std::string name,
                 bool alias = false,
                 bool global = false)
            : index(idx), file(fle), function(fcn), potentialAlias(alias), variableName(name), isGlobal(global) {
        dereferenced = false;
        visited = false;
    }

    /*
     * 何番目の引数として呼ばれたかを保持するらしい
     * プログラム中で何度か書き換えられると思うと気持ち悪いな...
     */
    unsigned int index;

    std::unordered_set<std::string>::iterator lastInsertedAlias;
    bool potentialAlias;
    bool dereferenced;
    bool isGlobal;

    /**
     * 手続き間の関係を計算済みのときtrueにするフラグと思われる
     */
    bool visited;

    /**
     * その変数が存在するソースファイルのパス
     */
    FileData file;

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
     * defされる行番号の集合
     */
    std::set<DefUseData> def;

    /**
     * useされる行番号の集合
     */
    std::set<DefUseData> use;

    /**
     * cfuncs{}
     */
    std::set<CFuncData> cfunctions;

    /**
     * dvars{}
     */
    std::set<DvarData> dvars;

    /**
     * pointers{}
     */
    std::unordered_set<std::string> aliases;

    /**
     * 変数IDを計算して返します。計算済みの場合はその値を即座に返します。
     * @return
     */
    std::string computeVariableId();

private:
    std::string id;
};
