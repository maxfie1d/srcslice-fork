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

#include <srcSliceHandler.hpp>


/**
 * 変数名からその変数のSliceProfileを検索して返す
 *Find function takes a variable name and searches first the local table to the function it was in
 *and if it's not found there, then it searches the global table to see if it's in there.
 *If neither has the variable, it returns null otherwise returns a pointer to the slice profile
 *@param varName - Name of the variable whose slice profile we want.
 *@return Pointer to Slice Profile or null.
 */
SliceProfile *srcSliceHandler::Find(const std::string &varName) {
    auto sp = FunctionVarMapItr->second.find(varName);
    if (sp != FunctionVarMapItr->second.end()) {
        return &(sp->second);
    } else { //check global map
        auto sp2 = sysDict->globalMap.find(varName);
        if (sp2 != sysDict->globalMap.end()) {
            return &(sp2->second);
        }
    }
    return nullptr;
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
        this->_logger->debug("dvars#1: {}", varIt->second.variableName);
        this->insertDvar(sp, varIt->second.variableName);
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
    if (currentDeclInit.name.empty()) { return; } //No nameless profiles.
    auto sp = Find(currentDeclInit.name);
    if (sawnew) { sawnew = false; }
    if (sp) {
        this->_logger->debug("use#1: {}", currentDeclInit.lineNumber);
        this->insertUse(sp, currentDeclInit.lineNumber);
        //new operator of the form int i = new int(tmp); screws around with aliasing
        if (varIt->second.potentialAlias && !sawnew) {
            varIt->second.lastInsertedAlias = varIt->second.aliases.insert(sp->variableName).first;
        } else {
            // dvars{} と use{} に追加する
            this->_logger->debug("dvars#2: {}", varIt->second.variableName);
            this->insertDvar(sp, varIt->second.variableName);
            this->_logger->debug("use#2: {}", currentDeclInit.lineNumber);
            this->insertUse(sp, currentDeclInit.lineNumber);
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
            varIt = FunctionVarMapItr->second.insert(
                    std::make_pair(currentDeclInit.name, std::move(currentSliceProfile))).first;
            this->_logger->debug("def#1: {}", currentDeclInit.lineNumber);
            this->insertDef(&varIt->second, currentDeclInit.lineNumber);
        }
    }

    // 現在の宣言初期化をクリア
    currentDeclInit.name.clear();
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
            // cfuncsを追加しようとしている
            // 関数呼び出しで使われる変数のslice-profileがあるか?
            // check to find sp for the variable being called on fcn
            auto sp = this->Find(callArgDataStack.top().name);
            if (sp) {
                // useを追加
                this->_logger->debug("use#3(関数の引数として使用されたため): {}", callArgDataStack.top().lineNumber);
                this->insertUse(sp, callArgDataStack.top().lineNumber);

                sp->index = argIndex;
                // cfuncstionを追加する(1/1)
                auto &calledFunctionName = nameOfCurrentCalledFunctionStack.top();
                this->_logger->debug("cfuncs(1/1): {}", calledFunctionName);
                this->insertCFunc(sp, calledFunctionName, argIndex, lineNum);
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
        currentSliceProfile.file = fileName;
        currentSliceProfile.function = functionTmplt.functionName;
        currentSliceProfile.variableName = currentParam.name;
        currentSliceProfile.potentialAlias = potentialAlias;
        currentSliceProfile.isGlobal = inGlobalScope;

        // std::move はやや難しいが、短くいうと所有権を移動する役割がある
        // 参考: http://kaworu.jpn.org/cpp/std::move

        // function-var-mapに新しく追加する
        varIt = FunctionVarMapItr->second.insert(
                std::make_pair(currentParam.name, std::move(currentSliceProfile))).first;
        // def{} に引数の行番号を追加する
        this->_logger->debug("def#2: {}", currentParam.lineNumber);
        this->insertDef(&varIt->second, currentParam.lineNumber);

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
        currentSliceProfile.file = fileName;
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
        currentSliceProfile.index = declIndex;
        currentSliceProfile.file = fileName;
        currentSliceProfile.function = functionTmplt.functionName;
        currentSliceProfile.variableName = currentDecl.name;
        currentSliceProfile.potentialAlias = potentialAlias;
        currentSliceProfile.isGlobal = inGlobalScope;

        // extern 宣言ならスキップする
        if (currentDeclSpecifier.name != "extern") {
            this->_logger->debug("ここやん！: {}, {}", currentSliceProfile.variableName, inGlobalScope);
            if (!inGlobalScope) {
                auto pair = std::make_pair(currentSliceProfile.variableName, std::move(currentSliceProfile));
                varIt = FunctionVarMapItr->second.insert(pair).first;
                // def{} 現在の宣言の行番号を追加する
                this->_logger->debug("def#3: {}", currentDecl.lineNumber);

                this->insertDef(&varIt->second, currentDecl.lineNumber);
            } else {
                //TODO: Handle def use for globals
                // グローバルマップに追加
                currentSliceProfile.function = "__GLOBAL__";
                this->insertDef(&currentSliceProfile, currentDecl.lineNumber);
                auto varmap_pair = std::make_pair(currentSliceProfile.variableName, std::move(currentSliceProfile));
                sysDict->globalMap.insert(varmap_pair);
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
    if (!lhsExprStmt.name.empty()) {
        SliceProfile *lhs = Find(lhsExprStmt.name);
        if (!lhs) {
            // 新しく左辺のslice-profileを作成しストアする
            currentSliceProfile.index = 1;
            currentSliceProfile.file = fileName;
            currentSliceProfile.function = functionTmplt.functionName;
            currentSliceProfile.variableName = lhsExprStmt.name;
            currentSliceProfile.potentialAlias = false;
            currentSliceProfile.isGlobal = inGlobalScope;

            this->_logger->debug("ここかな? #7: {}", currentSliceProfile.variableName);
            varIt = FunctionVarMapItr->second.insert(
                    std::make_pair(lhsExprStmt.name, std::move(currentSliceProfile))).first;
            this->_logger->debug("def#4: {}", lhsExprStmt.lineNumber);

            this->insertDef(&varIt->second, lhsExprStmt.lineNumber);
        } else {
            // 左辺のdef{}に追加する
            this->_logger->debug("def#5: {}", lhsExprStmt.lineNumber);

            this->insertDef(lhs, lhsExprStmt.lineNumber);
        }
    }
}

/**
 * ProcessExprStmtPostAssign
 */
void srcSliceHandler::ProcessExprStmtPostAssign() {
    SliceProfile *lhs = Find(lhsName);
    if (!lhs) {
        return;
    } else {
        // 右辺のslice-profileを検索する
        //find the sp for the rhs
        auto sprIt = Find(currentExprStmt.name);
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
//                sprIt->dvars.insert(lhs->variableName);
            } else {
                // it is an alias, so save that this is the most recent alias and insert it into rhs alias list
                // エイリアスなので、最も最近のエイリアスを右辺のエイリアスリストに追加して保存する
                lhs->lastInsertedAlias = lhs->aliases.insert(sprIt->variableName).first;
            }
            this->_logger->debug("use#4: {}", currentExprStmt.lineNumber);
            this->insertUse(sprIt, currentExprStmt.lineNumber);
            // ひとつにまとめます。もし他のもののエイリアスであるなら、もう一方を更新します。
            //Union things together. If this was an alias of anoter thing, update the other thing
            if (sprIt->potentialAlias && !dereferenced) {
                if (!sprIt->aliases.empty()) {
                    // 問題は最後のエイリアスがイテレータであり他の関数で参照できてしまうことです。
                    // おそらくポインタに加工します。なぜ必要か分かりました。
                    //problem  because last alias is an iterator and can reference things in other functions.
                    //Maybe make into a pointer. Figure out why I need it.
                    auto spaIt = FunctionVarMapItr->second.find(*(sprIt->lastInsertedAlias));
                    if (spaIt != FunctionVarMapItr->second.end()) {
                        this->_logger->debug("dvars#4: {}", lhs->variableName);
                        this->insertDvar(sprIt, lhs->variableName);
                        this->_logger->debug("use#5: {}", currentExprStmt.lineNumber);
                        this->insertUse(&spaIt->second, currentExprStmt.lineNumber);
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
        SliceProfile *useProfile = Find(pair.name);
        if (useProfile) {
            // 他の2つの式文の関数と同様同じ語に対して実行しています。
            //it's running on the same word as the other two exprstmt functions
            // use{} に追加
            this->_logger->debug("use#6: {}", pair.lineNumber);
            this->insertUse(useProfile, pair.lineNumber);
        }
    }
}

/**
 * ProcessDeclCtor
 * 宣言コンストラクタ?を処理します。
 */
void srcSliceHandler::ProcessDeclCtor() {
    SliceProfile *lhs = Find(currentDecl.name);
    if (!lhs) {
        return;
    } else {
        this->_logger->debug("use#7: {}", currentDecl.lineNumber);
        this->insertUse(lhs, currentDecl.lineNumber);
        SliceProfile *rhs = Find(currentDeclCtor.name);
        if (rhs) {
            this->_logger->debug("dvars#5: {}", lhs->variableName);

            // 検索する
            this->insertDvar(rhs, lhs->variableName);
//            rhs->dvars.insert(lhs->variableName);
            this->_logger->debug("use#8: {}", currentDecl.lineNumber);
            this->insertUse(rhs, currentDecl.lineNumber);
        }
    }
}

/**
 * ConputeInterprocedural
 * よく分からない。
 */
/**
 * ComputeInterprocedural
 * @param file_apth File name
 * No return value
 */
void srcSliceHandler::ComputeInterprocedural(const std::string &file_apth) {
    FileIt = sysDict->ffvMap.getFunctionVarMap(file_apth);
    if (FileIt == sysDict->ffvMap.getFileFunctionVarMap()->end()) {
        std::cerr << "CAN'T FIND FILE" << std::endl;
        return;
    } else {
        for (FunctionVarMapItr = FileIt->second.begin();
             FunctionVarMapItr != FileIt->second.end(); ++FunctionVarMapItr) {
            for (auto it = FunctionVarMapItr->second.begin(); it != FunctionVarMapItr->second.end(); ++it) {
                if (!it->second.visited) {
                    //std::unordered_set<NameAndLineNumber, NameLineNumberPairHash>::iterator - auto
                    for (auto itCF = it->second.cfunctions.begin(); itCF != it->second.cfunctions.end(); ++itCF) {
                        SliceProfile Spi = ArgumentProfile(itCF->calledFunctionName, itCF->argIndenx, it);
                        SetUnion(it->second.use, Spi.def);
                        SetUnion(it->second.use, Spi.use);
                        SetUnion(it->second.cfunctions, Spi.cfunctions);
                        SetUnion(it->second.dvars, Spi.dvars);
                        //SetUnion(it->second.aliases, Spi.aliases); //I suspect this is wrong, but I'll leave it here in case I'm wrong.
                    }
                    it->second.visited = true;
                }
            }
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
SliceProfile srcSliceHandler::ArgumentProfile(std::string fname, unsigned int parameterIndex, VarMap::iterator vIt) {
    //TODO varIt is a hack here. Fix. We shouldn't need to pass an extra param to do this.

    SliceProfile Spi;
    auto gFuncIt = sysDict->functionTable.findByName(fname);
    if (gFuncIt) {
        FileIt = sysDict->ffvMap.find(gFuncIt->fileName);
    }
    auto funcIt = FileIt->second.find(fname);
    if (funcIt != FileIt->second.end()) {
        for (auto it = funcIt->second.begin(); it != funcIt->second.end(); ++it) {
            if (it->second.index == (parameterIndex)) {
                if (it->second.visited) {
                    if (it->second.potentialAlias) {
                        it->second.aliases.insert(vIt->second.variableName);
                    }
                    return it->second;
                } else {
                    for (auto itCF = it->second.cfunctions.begin(); itCF != it->second.cfunctions.end(); ++itCF) {
                        std::string newFunctionName = itCF->calledFunctionName;
                        unsigned int newParameterIndex = itCF->argIndenx;
                        if (newFunctionName != fname) {
                            it->second.visited = true;
                            Spi = ArgumentProfile(newFunctionName, newParameterIndex, it);
                            SetUnion(it->second.use, Spi.def);
                            SetUnion(it->second.use, Spi.use);
                            SetUnion(it->second.cfunctions, Spi.cfunctions);
                            SetUnion(it->second.dvars, Spi.dvars);
                            SetUnion(it->second.aliases, Spi.aliases);
                        }
                    }
                    if (it->second.potentialAlias) {
                        it->second.aliases.insert(vIt->second.variableName);
                    }
                    it->second.visited = true;
                    return it->second;
                }
            }
        }
    } else {
        //uncomment to see errors. Need to handle this properly. Also, currently, a lot of these misses are calls 
        //into functions in header files or libraries that I can't see. Running preprocessor before srcML should
        //help but will also make things messy.
        //std::cout<<"ERROR IN ARGUMENT PROFILE WHEN ACCESSING: "<<fname<<std::endl;
    }
    return Spi;
}

std::string srcSliceHandler::getFunctionId(unsigned int lineNumber) {
    if (this->fileName.empty() || this->functionTmplt.functionName.empty()) {
        return "file: " + fileName + ", func: " + this->functionTmplt.functionName;
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
    // TODO: 本来は辞書を検索してSliceProfileのインスタンスを検索するべき

    if (this->fileName.empty() || this->functionTmplt.functionName.empty()) {
        return "<unknown variable id>";
    } else {
        SliceProfile sp;
        sp.file = this->fileName;
        sp.function = this->functionTmplt.functionName;
        sp.variableName = variableName;
        return sp.computeVariableId();
    }
}

/**
 * SliceProfileのdefを追加します
 * @param sp
 * @param lineNumber
 */
void srcSliceHandler::insertDef(SliceProfile *sp, unsigned int lineNumber) {
    sp->def.insert(ProgramPoint(lineNumber, this->getFunctionId(lineNumber)));

}

/**
 * SliceProfileのuseを追加します
 * @param sp
 * @param lineNumber
 */
void srcSliceHandler::insertUse(SliceProfile *sp, unsigned int lineNumber) {
    sp->use.insert(ProgramPoint(lineNumber, this->getFunctionId(lineNumber)));
}

void srcSliceHandler::insertDvar(SliceProfile *sp, std::string variableName) {
    sp->dvars.insert(DvarData(variableName, this->getVariableId(variableName)));
}

void srcSliceHandler::insertCFunc(SliceProfile *sp,
                                  std::string calledFunctionName,
                                  unsigned short argIndex,
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
