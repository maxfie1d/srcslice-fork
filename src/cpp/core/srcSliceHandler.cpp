/**
 * @file srcSliceHandler.cpp
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

#include <core/srcSliceHandler.hpp>
#include <helpers/functional.hpp>


/**
 * 変数名からその変数のSliceProfileを検索して返す
 *Find function takes a variable name and searches first the local table to the function it was in
 *and if it's not found there, then it searches the global table to see if it's in there.
 *If neither has the variable, it returns null otherwise returns a pointer to the slice profile
 *@param varName - Name of the variable whose slice profile we want.
 *@return Pointer to Slice Profile or null.
 */
SliceProfile *srcSliceHandler::Find(std::string varName) {
    // 構造体の場合は
    // メンバ名を削った部分で検索する
    std::string search_word = extractObjectName(varName);

    auto sp = varmapFind(search_word, p_varMap);
    if (sp) {
        return sp;
    } else {
        //check global map
        auto sp2 = sysDict->variableTable.findGlobalVariableSliceProfileByName(search_word);
        return sp2;
    }
}

/**
 * 関係なし
 */
/**
 *ProcessConstructorDecl
 *Processes decls of the form object(arg,arg)
 */
void srcSliceHandler::ProcessConstructorDecl() {
    auto sp = Find(currentDeclArg.name);
    if (sp) {
        this->_logger->debug("dvars#1: {}", p_sliceProfile->variableName);
        this->insertDvar(sp, p_sliceProfile->variableName);
    }
}

/**
 * 宣言文を処理する関数。
 * 宣言文の右辺を取って処理します。トークンに分割してマップに対して投げます。
 * 例外的に、new演算子はその引数がエイリアスだとしてもオブジェクトを作ります。
 * unordered_mapのキーと値はitr->first, itr->secondで取得できる
 */
/*
*ProcessDeclStmt
*Takes rhs of decl_stmt and processes it. Split by tokens and then throw against the map for an answer.
*corner case at new operator because new makes an object even if its argument is an alias.
*/
void srcSliceHandler::ProcessDeclStmt() {
    std::string current_decl_init_name = currentDeclInit.name;
    if (!current_decl_init_name.empty()) {
        auto sp = Find(current_decl_init_name);
        if (sawnew) { sawnew = false; }
        if (sp) {
            this->_logger->debug("use#1: {}", currentDeclInit.lineNumber);
            this->insertUse(sp, currentDeclInit.lineNumber, current_decl_init_name);
            //new operator of the form int i = new int(tmp); screws around with aliasing
            if (p_sliceProfile->potentialAlias && !sawnew) {
                p_sliceProfile->lastInsertedAlias = p_sliceProfile->aliases.insert(sp->variableName).first;
            } else {
                // dvars{} と use{} に追加する
                this->_logger->debug("dvars#2: {}", p_sliceProfile->variableName);
                this->insertDvar(sp, p_sliceProfile->variableName);
                this->_logger->debug("use#2: {}", currentDeclInit.lineNumber);
                this->insertUse(sp, currentDeclInit.lineNumber, current_decl_init_name);
            }
        } else {
            // for ループにおける初期化(init)は一般的な宣言文のハンドリングと異なる
            // これによって、見逃された奇妙な初期化(init)を掴み取る
            // これは for ループの初期化にのみ関係がある
            // Inits in for loops screw with typical decl statement handling.
            // This will grab decls I miss due to weird init usage
            // This really is only for for loop inits.
            // TODO: Possibly, write a separate function for these.
            if (!inFor) {
                return;
            } else {
                auto pair = std::make_pair(current_decl_init_name, std::move(currentSliceProfile));
                p_sliceProfile = &p_varMap->insert(pair).first->second;
                this->_logger->debug("def#1: {}", currentDeclInit.lineNumber);
                this->insertDef(p_sliceProfile, currentDeclInit.lineNumber, current_decl_init_name);
            }
        }

        // 現在の宣言初期化をクリア
        currentDeclInit.name.clear();
    }
}

/**
 * GetCallData
 * 現在呼び出している関数の引数の名前を取得するために正確なコンストレインを知る。
 * それらに対してslice-profileを見つけることができたらそれらの変数についてのデータをストアします。
 * 本質的には新しいデータを反映するために引数のslice-profileを更新する
 */
/*
* GetCallData
* Knows the proper constrains for obtaining the name of arguments of currently called function
* It stores data about those variables if it can find a slice profile entry for them.
* Essentially, update the slice profile of the argument to reflect new data.
*/
void srcSliceHandler::GetCallData() {
    // 関数の引数を取得する
    //Get function arguments
    if (triggerField[argument_list] &&
        triggerField[argument] &&
        triggerField[expr] &&
        triggerField[name] &&
        !nameOfCurrentCalledFunctionStack.empty()) {
        if (!callArgDataStack.empty()) {
            std::string name = callArgDataStack.top().name;
            // cfuncsを追加しようとしている
            // 関数呼び出しで使われる変数のslice-profileがあるか?
            // check to find sp for the variable being called on fcn
            auto sp = this->Find(name);
            if (sp) {
                // useを追加
                this->_logger->debug("use#3(関数の引数として使用されたため): {}, {}", callArgDataStack.top().lineNumber, name);
                this->insertUse(sp, callArgDataStack.top().lineNumber, name);

                sp->index = argIndex;

                // cfuncstionを追加する(1/1)
                auto &calledFunctionName = nameOfCurrentCalledFunctionStack.top();
                this->_logger->debug("cfuncs(1/1): {}", calledFunctionName);
                ArgIndexAndMemberName a(argIndex,
                                        extractMemberName(callArgDataStack.top().name));
                this->insertCFunc(sp, calledFunctionName, std::move(a), lineNum);
            }
        }
    }
}

void srcSliceHandler::GetParamType() {
    // 現在のslice-profileに変数の型をセットする
    currentSliceProfile.variableType = currentParamType.name;
    currentParamType.name.clear();
    currentTypeSpecifier.name.clear();
}

/**
 * GetParamName
 * ParamNameという名前だが、そういう動作には見えない...
 */
void srcSliceHandler::GetParamName() {
    // 現在のslice-profileのインデックス、ファイル名、関数名、変数名などをセットする

    // int main(void)などの場合でパラメータが"void"であるとき
    // パラメータは空なのでスキップする
    if (currentSliceProfile.variableType != "void") {
        currentSliceProfile.index = declIndex;
        currentSliceProfile.file = FileData(fileName, lineNum);
        currentSliceProfile.function = functionTmplt.functionName;
        currentSliceProfile.variableName = currentParam.name;
        currentSliceProfile.potentialAlias = potentialAlias;
        currentSliceProfile.isGlobal = inGlobalScope;

        // std::move はやや難しいが、短くいうと所有権を移動する役割がある
        // 参考: http://kaworu.jpn.org/cpp/std::move

        // function-var-mapに新しく追加する
        auto pair = std::make_pair(currentParam.name, std::move(currentSliceProfile));
        p_sliceProfile = &p_varMap->insert(pair).first->second;
        // def{} に引数の行番号を追加する
        this->_logger->debug("def#2: {}", currentParam.lineNumber);
        this->insertDef(p_sliceProfile, currentParam.lineNumber, currentParam.name);

        currentParam.name.clear();
    }
}

/**
 * GetFunctionData
 * 関数の返り値の型、名前、引数を取得するために正確なコンストレインを知る。
 * これらの情報はすべて一時的な関数情報として保持する
 */
/**
* GetFunctionData
* Knows proper constraints for obtaining function's return type, name, and arguments.
* Stores all of this in functionTmplt.
*/
void srcSliceHandler::GetFunctionData() {
    //Get function name
    if (isConstructor) {
        std::stringstream ststrm;
        ststrm << constructorNum;
        currentFunctionBody.name += ststrm.str(); //number the constructor. Find a better way than stringstreams someday.
    }
    functionTmplt.functionName = currentFunctionBody.name;
    currentFunctionBody.name.clear();
}

/**
 * GetFunctionDeclData
 * 関数宣言データを取得する
 */
void srcSliceHandler::GetFunctionDeclData() {
    functionTmplt.params.push_back(currentParamType.name);
}

/**
 * 現在のsrc-profileに各データをセットする
 */
void srcSliceHandler::AssignProfile() {
    if (currentSliceProfile.index == 0) {
        currentSliceProfile.index = declIndex;
    }
    if (currentSliceProfile.file.empty()) {
        currentSliceProfile.file = FileData(fileName, lineNum);
    }
    if (currentSliceProfile.function.empty()) {
        currentSliceProfile.function = functionTmplt.functionName;
    }
    if (currentSliceProfile.variableName.empty()) {
        currentSliceProfile.variableName = currentDecl.name;
        //std::cerr<<currentDecl.first<<std::endl;
    }
    if (!currentSliceProfile.potentialAlias) {
        currentSliceProfile.potentialAlias = potentialAlias;
    }
    if (!currentSliceProfile.isGlobal) {
        currentSliceProfile.isGlobal = inGlobalScope;
    }
}


/**
 * GetDeclStmtData
 * 宣言文(変数宣言文など)のデータを取得する
 * 宣言文の型と名前を取得するために正確なコンストレインを知る。
 * 新しいslice-profileを作成し、宣言文についての情報をストアする。
* GetDeclStmtData
* Knows proper constraints for obtaining DeclStmt type and name.
* creates a new slice profile and stores data about decl statement inside.
*/
void srcSliceHandler::GetDeclStmtData() {
    if (currentDecl.name.empty()) {
        return;
    } else {
        auto &csp = currentSliceProfile;

        csp.index = declIndex;
        csp.file = FileData(fileName, lineNum);
        csp.function = functionTmplt.functionName;
        csp.variableName = currentDecl.name;
        csp.potentialAlias = potentialAlias;
        csp.isGlobal = inGlobalScope;

        std::string var_name = csp.variableName;

        if (currentDeclSpecifier.name != "extern") {
            this->_logger->debug("ここやん！: {}, {}", var_name, inGlobalScope);
            if (!inGlobalScope) {
                auto pair = std::make_pair(var_name, std::move(currentSliceProfile));
                p_sliceProfile = &p_varMap->insert(pair).first->second;
                // def{} 現在の宣言の行番号を追加する
                this->_logger->debug("def#3: {}", currentDecl.lineNumber);
                this->insertDef(p_sliceProfile, currentDecl.lineNumber, var_name);
            } else {
                // グローバルマップに追加
                auto g_sp = sysDict->variableTable.findGlobalVariableSliceProfileByName(var_name);
                if (!g_sp) {
                    currentSliceProfile.function = "__GLOBAL__";
                    this->insertDef(&currentSliceProfile, currentDecl.lineNumber, var_name);
                    auto varmap_pair = std::make_pair(var_name, std::move(csp));
                    sysDict->variableTable.addGlobalVariable(varmap_pair);
                }
            }
        } else {
            // extern 宣言の場合は変数表にガワだけ登録する
            if (inGlobalScope) {
                csp.function = "__GLOBAL__";
                auto varmap_pair = std::make_pair(var_name, std::move(csp));
                sysDict->variableTable.addGlobalVariable(varmap_pair);
            }
        }
        currentDecl.name.clear();
    }
}

/**
 * ProcessExprStmt
 * 式文(2017.10.29: おそらく代入文のことだと思われる)全体を取得し、はじめに左辺と右辺に分けることにより処理します。
 * マップの中に見つかった場合左辺の行番号を保存するように処理します。
 * 左辺を処理したら、alias, dvars, 関数呼び出しのために右辺を処理します。
 *
 * ProcessExprStmtPreAssign
 * Get entire expression statement and then process by first splitting to lhs and rhs.
 * Process the lhs by saving its slines if it can be found in the map.
 * After lhs is processed, keep track of it and then process the rhs
 * for any aliases, dvars, or function calls.
 */
void srcSliceHandler::ProcessExprStmtPreAssign() {
    std::string lhs_name = lhsExprStmt.name;
    unsigned int lhs_line = lhsExprStmt.lineNumber;

    if (!lhsExprStmt.name.empty()) {
        auto lhs_sp = Find(lhs_name);
        if (!lhs_sp) {
            // 新しく左辺のslice-profileを作成しストアする
            currentSliceProfile.index = 1;
            currentSliceProfile.file = FileData(fileName, lineNum);
            currentSliceProfile.function = functionTmplt.functionName;
            currentSliceProfile.variableName = lhsExprStmt.name;
            currentSliceProfile.potentialAlias = false;
            currentSliceProfile.isGlobal = inGlobalScope;

            this->_logger->debug("ここかな? #7: {}", currentSliceProfile.variableName);
            auto pair = std::make_pair(lhsExprStmt.name, std::move(currentSliceProfile));
            p_sliceProfile = &p_varMap->insert(pair).first->second;
            this->_logger->debug("def#4: {}", lhs_line);
            this->insertDef(p_sliceProfile, lhs_line, lhs_name);
        } else {
            this->_logger->debug("def#5: {}, {}", lhs_line, lhs_name);
            this->insertDef(lhs_sp, lhs_line, lhs_name);
        }
    }
}

/**
 * ProcessExprStmtPostAssign
 */
void srcSliceHandler::ProcessExprStmtPostAssign() {
    auto lhs = Find(lhsName);
    if (!lhs) {
        return;
    } else {
        // 右辺のslice-profileを検索する
        //find the sp for the rhs
        std::string name = currentExprStmt.name;
        auto sprIt = Find(name);
        if (sprIt) {
            // 左値は右値に依存する
            //lvalue depends on this rvalue
            if (!lhs->potentialAlias || dereferenced) {
                // 潜在的には参照ではない。もしそうなら、被参照されてはならない。
                // エイリアスではないので、dvarである
                //It is not potentially a reference and if it is, it must not have been dereferenced
                //it's not an alias so it's a dvar
                this->_logger->debug("dvars#3: {}", lhs->variableName);
                this->insertDvar(sprIt, lhs->variableName);
            } else {
                // it is an alias, so save that this is the most recent alias and insert it into rhs alias list
                // エイリアスなので、最も最近のエイリアスを右辺のエイリアスリストに追加して保存する
                lhs->lastInsertedAlias = lhs->aliases.insert(sprIt->variableName).first;
            }
            this->_logger->debug("use#4: {}, {}", currentExprStmt.lineNumber, name);
            this->insertUse(sprIt, currentExprStmt.lineNumber, name);
            this->_logger->debug("sprIt: {}", sprIt->variableName);

            // ひとつにまとめます。もし他のもののエイリアスであるなら、もう一方を更新します。
            //Union things together. If this was an alias of anoter thing, update the other thing
            if (sprIt->potentialAlias && !dereferenced) {
                if (!sprIt->aliases.empty()) {
                    // 問題は最後のエイリアスがイテレータであり他の関数で参照できてしまうことです。
                    // おそらくポインタに加工します。なぜ必要か分かりました。
                    //problem  because last alias is an iterator and can reference things in other functions.
                    //Maybe make into a pointer. Figure out why I need it.
                    auto spaIt = p_varMap->find(*(sprIt->lastInsertedAlias));
                    if (spaIt != p_varMap->end()) {
                        this->_logger->debug("dvars#4: {}", lhs->variableName);
                        this->insertDvar(sprIt, lhs->variableName);
                        this->_logger->debug("use#5: {}", currentExprStmt.lineNumber);
                        this->insertUse(&spaIt->second, currentExprStmt.lineNumber, name);
                    }
                }
            }
        }
    }
}

/**
 * ProcessExprStmtNoAssign
 * 代入(assign)なしの式文を処理します。
 */
void srcSliceHandler::ProcessExprStmtNoAssign() {
    for (auto pair : useExprStack) {
        auto useProfile = Find(pair.name);
        if (useProfile) {
            // 他の2つの式文の関数と同様同じ語に対して実行しています。
            //it's running on the same word as the other two exprstmt functions
            // use{} に追加
            this->_logger->debug("use#6: {}, {}", pair.lineNumber, pair.name);
            this->insertUse(useProfile, pair.lineNumber, pair.name);
        }
    }
}

/**
 * ProcessDeclCtor
 * 宣言コンストラクタ?を処理します。
 */
void srcSliceHandler::ProcessDeclCtor() {
    std::string name = currentDecl.name;
    auto lhs = Find(name);
    if (!lhs) {
        return;
    } else {
        this->_logger->debug("use#7: {}", currentDecl.lineNumber);
        this->insertUse(lhs, currentDecl.lineNumber, name);
        auto rhs = Find(currentDeclCtor.name);
        if (rhs) {
            this->_logger->debug("dvars#5: {}", lhs->variableName);

            // 検索する
            this->insertDvar(rhs, lhs->variableName);
            this->_logger->debug("use#8: {}", currentDecl.lineNumber);
            this->insertUse(rhs, currentDecl.lineNumber, name);
        }
    }
}

/**
 * ファイル名、パラメータインデックス、VarMapのイテレータを引数に
 * その変数のslice-profileを返します
 */
/*
 *ArgumentProfile
 *@param functIt iterator to the FunctionVarMap
 *@param parameterIndex index of the parameter
 *@Return SliceProfile of the variable
*/
SliceProfile
srcSliceHandler::createArgumentSp(std::string func_name, ArgIndexAndMemberName parameterIndex) {
    // 関数名で関数テーブルを検索してみる
    auto gFuncIt = sysDict->functionTable.findByName(func_name);
    if (gFuncIt) {
        // 関数テーブルにその関数があれば、その関数の所属する関数変数マップを取得する
        p_functionVarMap = sysDict->variableTable.findFunctionVarMap(gFuncIt->fileName);
    }
    // 関数名でその関数の変数マップを取得する
    auto varmap_pair = p_functionVarMap->find(func_name);
    if (varmap_pair != p_functionVarMap->end()) {
        // それぞれの変数のSliceProfileについて
        for (auto &sp_pair : varmap_pair->second) {
            auto &sp = sp_pair.second;
            if (sp.index == parameterIndex.index) {
                if (sp.visited) {
                    return sp;
                } else {
                    // ここで再帰的に連鎖を繋いでいると思われる
                    for (auto &itCF: sp.cfunctions) {
                        std::string newFunctionName = itCF.calledFunctionName;
                        auto newParameterIndex = itCF.argIndenx;
                        if (newFunctionName != func_name) {
                            sp.visited = true;
                            auto Spi = createArgumentSp(newFunctionName, newParameterIndex);
                            combineElements(&sp.def, &Spi.def);
                            combineElements(&sp.use, &Spi.use);
                            combineElements(&sp.cfunctions, &Spi.cfunctions);
                            combineElements(&sp.dvars, &Spi.dvars);
                            combineElements(&sp.aliases, &Spi.aliases);
                        }
                    }
                    sp.visited = true;
                    return sp;
                }
            }
        }
    } else {
        //uncomment to see errors. Need to handle this properly. Also, currently, a lot of these misses are calls
        //into functions in header files or libraries that I can't see. Running preprocessor before srcML should
        //help but will also make things messy.
        //std::cout<<"ERROR IN ARGUMENT PROFILE WHEN ACCESSING: "<<fname<<std::endl;
    }
    return SliceProfile();
}

std::string srcSliceHandler::getFunctionId(unsigned int lineNumber) {
    if (this->fileName.empty() || this->functionTmplt.functionName.empty()) {
        return "__GLOBAL__";
    } else {
        // idを作る
        // NOTE: 関数テーブルから関数を検索してという方法もあるが
        // 関数テーブルに関数が登録されるタイミングが関数が「閉じた」時だったので
        // その場で関数IDだけを計算することにした
        FunctionData fd;
        fd.fileName = this->fileName;
        fd.functionName = this->functionTmplt.functionName;
        return fd.computeId();
    }
}

std::string srcSliceHandler::getVariableId(std::string variableName) {
    auto a = this->sysDict->variableTable.findSliceProfile(this->fileName, this->functionTmplt.functionName,
                                                           variableName);
    if (a) {
        return a->computeVariableId();
    } else {
        // なければグローバル変数で探す
        auto b = this->sysDict->variableTable.findGlobalVariableSliceProfileByName(variableName);
        if (b) {
            return b->computeVariableId();
        } else {
            return "<unknown variable id>";
        }
    }

    // Issue #26のために辞書を探索して見つけたSliceProfileをもとに
    // IDを取得する方法に切り替えた
//    if (this->fileName.empty() || this->functionTmplt.functionName.empty()) {
//        return "<unknown variable id>";
//    } else {
//        SliceProfile sp;
//        sp.file = this->fileName;
//        sp.function = this->functionTmplt.functionName;
//        sp.variableName = variableName;
//        return sp.computeVariableId();
//    }
}

void srcSliceHandler::insertDef(SliceProfile *sp, unsigned int lineNumber, std::string name) {
    auto pos = name.find('.');
    ProgramPoint pp(lineNumber, this->getFunctionId(lineNumber));
    if (pos == std::string::npos) {
        sp->def.insert(DefUseData(pp));
    } else {
        auto member_name_length = (name.length() - 1) - pos;
        std::string member_name = name.substr(pos + 1, member_name_length);
        sp->def.insert(DefUseData(pp, member_name));
    }
}

void srcSliceHandler::insertUse(SliceProfile *sp, unsigned int lineNumber, std::string name) {
    auto pos = name.find('.');
    ProgramPoint pp(lineNumber, this->getFunctionId(lineNumber));
    if (pos == std::string::npos) {
        sp->use.insert(DefUseData(pp));
    } else {
        auto member_name_length = (name.length() - 1) - pos;
        std::string member_name = name.substr(pos + 1, member_name_length);
        sp->use.insert(DefUseData(pp, member_name));
    }
}

void srcSliceHandler::insertDvar(SliceProfile *sp, std::string variableName) {
    sp->dvars.insert(DvarData(variableName, this->getVariableId(variableName)));
}

void srcSliceHandler::insertCFunc(SliceProfile *sp,
                                  std::string calledFunctionName,
                                  ArgIndexAndMemberName argIndex,
                                  unsigned int lineNumber) {
    auto function_table = sysDict->functionTable;
    auto calledFunctionData = function_table.findByName(calledFunctionName);
    if (calledFunctionData) {
        auto calledFunctionId = calledFunctionData->computeId();
        sp->cfunctions.insert(CFuncData(
                calledFunctionName,
                calledFunctionData->computeId(),
                argIndex,
                ProgramPoint(lineNumber, this->getFunctionId(lineNumber))
        ));
    } else {
        // 関数表になければライブラリ関数ということにしている
        sp->cfunctions.insert(CFuncData(
                calledFunctionName,
                "<libfunc>",
                argIndex,
                ProgramPoint(lineNumber, this->getFunctionId(lineNumber))
        ));
    }
}

void srcSliceHandler::compute(SliceProfile &sp) {
    if (!sp.visited) {
        for (auto &cfunc : sp.cfunctions) {
            // cfuncのdef,use,cfunctions,dvarsと結合する
            auto argumentSp = this->createArgumentSp(cfunc.calledFunctionName,
                                                     cfunc.argIndenx);

            auto f = [&](std::set<DefUseData> dds) {
                return cfunc.argIndenx.member_name.empty()
                       ? dds
                       : set_transform<DefUseData, DefUseData>(dds, [&](DefUseData dd) {
                            dd.member_name = cfunc.argIndenx.member_name;
                            return dd;
                        });
            };
            auto def = f(argumentSp.def);
            auto use = f(argumentSp.use);
            combineElements(&sp.def, &def);
            combineElements(&sp.use, &use);
            combineElements(&sp.cfunctions, &argumentSp.cfunctions);
            combineElements(&sp.dvars, &argumentSp.dvars);
        }
        sp.visited = true;
    }
}

/**
 * ConputeInterprocedural
 * 関数呼び出しなどでuse{}に伝播などを計算する
 */
/**
 * ComputeInterprocedural
 * @param file_path File name
 * No return value
 */
void srcSliceHandler::ComputeInterprocedural(const std::string &file_path) {
    // ファイルは変数テーブル中に存在するか
    auto func_varmap = sysDict->variableTable.findFunctionVarMap(file_path);
    if (!func_varmap) {
        std::cerr << "変数テーブル中にそんなファイルないで: " << file_path << std::endl;
        return;
    } else {
        // それぞれの関数の変数マップについて
        for (auto &varmap_pair:*func_varmap) {
            // それぞれのSliceProfileについて
            for (auto &s_profile_pair : varmap_pair.second) {
                auto &sp = s_profile_pair.second;
                this->compute(sp);
            }
        }
    }
}

void srcSliceHandler::computeAllInterproceduralRelation() {
    for (auto pair : *this->sysDict->variableTable.getFileFunctionVarMap()) {
        std::string file_name = pair.first;
        this->ComputeInterprocedural(file_name);
    }

    // ここでグローバル変数も処理する
    for (auto &s_profile_pair :*this->sysDict->variableTable.getRawGlobalVariableTable()) {

        auto &sp = s_profile_pair.second;
        this->compute(sp);
    }
}
