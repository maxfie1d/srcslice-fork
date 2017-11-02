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
#include <Utility.hpp>



/**
 * 関係なし
 */
/**
 *Find
 *Find function takes a variable name and searches first the local table to the function it was in
 *and if it's not found there, then it searches the global table to see if it's in there.
 *If neither has the variable, it returns null otherwise returns a pointer to the slice profile
 *@param varName - Name of the variable whose slice profile we want.
 *@return Pointer to Slice Profile or null.
 */
SliceProfile *srcSliceHandler::Find(const std::string &varName) {
    auto sp = FunctionIt->second.find(varName);
    if (sp != FunctionIt->second.end()) {
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
    auto sp = Find(currentDeclArg.first);
    if (sp) {
        this->_logger->debug("dvars#1: {}", varIt->second.variableName);
        sp->dvars.insert(varIt->second.variableName);
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
    if (currentDeclInit.first == "") { return; } //No nameless profiles.
    auto sp = Find(currentDeclInit.first);
    if (sawnew) { sawnew = false; }
    if (sp) {
        varIt->second.slines.insert(currentDeclInit.second); //varIt is lhs
        this->_logger->debug("use#1: {}", currentDeclInit.second);
        sp->use.insert(currentDeclInit.second);
        //new operator of the form int i = new int(tmp); screws around with aliasing
        if (varIt->second.potentialAlias && !sawnew) {
            varIt->second.lastInsertedAlias = varIt->second.aliases.insert(sp->variableName).first;
        } else {
            // dvars{} と use{} に追加する
            this->_logger->debug("dvars#2: {}", varIt->second.variableName);
            sp->dvars.insert(varIt->second.variableName);
            this->_logger->debug("use#2: {}", currentDeclInit.second);
            sp->use.insert(currentDeclInit.second);
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
            varIt = FunctionIt->second.insert(
                    std::make_pair(currentDeclInit.first, std::move(currentSliceProfile))).first;
            this->_logger->debug("def#1: {}", currentDeclInit.second);
            varIt->second.def.insert(currentDeclInit.second);
        }
    }

    // 現在の宣言初期化をクリア
    currentDeclInit.first.clear();
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
        !nameOfCurrentClldFcn.empty()) {
        if (!callArgData.empty()) {
            // 関数呼び出しで使われる変数のslice-profileがあるか?
            //check to find sp for the variable being called on fcn
            auto sp = Find(callArgData.top().first);
            if (sp) {
                sp->slines.insert(callArgData.top().second);
                this->_logger->debug("use#3: {}", callArgData.top().second);
                sp->use.insert(callArgData.top().second);
                sp->index = numArgs;
                this->_logger->debug("cfuncs#1: {}", nameOfCurrentClldFcn.top());
                sp->cfunctions.insert(std::make_pair(nameOfCurrentClldFcn.top(), numArgs));
            }
        }
    }
}

void srcSliceHandler::GetParamType() {
    // 現在のslice-profileに変数の型をセットする
    currentSliceProfile.variableType = currentParamType.first;
    currentParamType.first.clear();
}

/**
 * GetParamName
 * ParamNameという名前だが、そういう動作には見えない...
 */
void srcSliceHandler::GetParamName() {
    // 現在のslice-profileのインデックス、ファイル名、関数名、変数名などをセットする
    currentSliceProfile.index = declIndex;
    currentSliceProfile.file = fileName;
    currentSliceProfile.function = functionTmplt.functionName;
    currentSliceProfile.variableName = currentParam.first;
    currentSliceProfile.potentialAlias = potentialAlias;
    currentSliceProfile.isGlobal = inGlobalScope;

    // std::move はやや難しいが、短くいうと所有権を移動する役割がある
    // 参考: http://kaworu.jpn.org/cpp/std::move

    // function-var-mapに新しく追加する
    varIt = FunctionIt->second.insert(std::make_pair(currentParam.first, std::move(currentSliceProfile))).first;
    // def{} に引数の行番号を追加する
    this->_logger->debug("def#2: {}", currentParam.second);
    varIt->second.def.insert(currentParam.second);

    currentParam.first.clear();
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
        currentFunctionBody.first += ststrm.str(); //number the constructor. Find a better way than stringstreams someday.
    }
    functionTmplt.functionName = currentFunctionBody.first;
    currentFunctionBody.first.clear();
}

/**
 * GetFunctionDeclData
 * 関数宣言データを取得する
 */
void srcSliceHandler::GetFunctionDeclData() {
    functionTmplt.params.push_back(currentParamType.first);
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
        currentSliceProfile.variableName = currentDecl.first;
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
 * 宣言文のデータを取得する
 * 宣言文の型と名前を取得するために正確なコンストレインを知る。
 * 新しいslice-profileを作成し、宣言文についての情報をストアする。
 */
/**
* GetDeclStmtData
* Knows proper constraints for obtaining DeclStmt type and name.
* creates a new slice profile and stores data about decl statement inside.
*/
void srcSliceHandler::GetDeclStmtData() {
    if (currentDecl.first.empty()) {
        return;
    } else {
        currentSliceProfile.index = declIndex;
        currentSliceProfile.file = fileName;
        currentSliceProfile.function = functionTmplt.functionName;
        currentSliceProfile.variableName = currentDecl.first;
        currentSliceProfile.potentialAlias = potentialAlias;
        currentSliceProfile.isGlobal = inGlobalScope;
        if (!inGlobalScope) {
            auto pair = std::make_pair(currentSliceProfile.variableName, std::move(currentSliceProfile));
            varIt = FunctionIt->second.insert(pair).first;
            // def{} 現在の宣言の行番号を追加する
            this->_logger->debug("def#3: {}", currentDecl.second);

            varIt->second.def.insert(currentDecl.second);
        } else {
            //TODO: Handle def use for globals
            sysDict->globalMap.insert(std::make_pair(currentSliceProfile.variableName, std::move(currentSliceProfile)));
        }
        currentDecl.first.clear();
    }
}

/**
 * ProcessExprStmt
 * 式文(2017.10.29: おそらく代入文のことだと思われる)全体を取得し、はじめに左辺と右辺に分けることにより処理します。
 * マップの中に見つかった場合左辺の行番号を保存するように処理します。
 * 左辺を処理したら、alias, dvars, 関数呼び出しのために右辺を処理します。
 */
/**
 * ProcessExprStmtPreAssign
 * Get entire expression statement and then process by first splitting to lhs and rhs.
 * Process the lhs by saving its slines if it can be found in the map.
 * After lhs is processed, keep track of it and then process the rhs
 * for any aliases, dvars, or function calls.
 */
void srcSliceHandler::ProcessExprStmtPreAssign() {
    if (!lhsExprStmt.first.empty()) {
        SliceProfile *lhs = Find(lhsExprStmt.first);
        if (!lhs) {
            // 新しく左辺のslice-profileを作成しストアする
            currentSliceProfile.index = -1;
            currentSliceProfile.file = fileName;
            currentSliceProfile.function = functionTmplt.functionName;
            currentSliceProfile.variableName = lhsExprStmt.first;
            currentSliceProfile.potentialAlias = false;
            currentSliceProfile.isGlobal = inGlobalScope;

            std::cout << "ここかな? #7: " << currentSliceProfile.variableName << std::endl;
            varIt = FunctionIt->second.insert(std::make_pair(lhsExprStmt.first, std::move(currentSliceProfile))).first;
            this->_logger->debug("def#4: {}", lhsExprStmt.second);

            varIt->second.def.insert(lhsExprStmt.second);
        } else {
            // 左辺のdef{}に追加する
            this->_logger->debug("def#5: {}", lhsExprStmt.second);

            lhs->def.insert(lhsExprStmt.second);
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
        auto sprIt = Find(currentExprStmt.first);
        if (sprIt) {
            // 左値は右値に依存する
            //lvalue depends on this rvalue
            if (!lhs->potentialAlias || dereferenced) {
                // 潜在的には参照ではない。もしそうなら、被参照されてはならない。
                // エイリアスではないので、dvarである
                //It is not potentially a reference and if it is, it must not have been dereferenced
                //it's not an alias so it's a dvar
                this->_logger->debug("dvars#3: {}", lhs->variableName);
                sprIt->dvars.insert(lhs->variableName);
            } else {
                // it is an alias, so save that this is the most recent alias and insert it into rhs alias list
                // エイリアスなので、最も最近のエイリアスを右辺のエイリアスリストに追加して保存する
                lhs->lastInsertedAlias = lhs->aliases.insert(sprIt->variableName).first;
            }
            this->_logger->debug("use#4: {}", currentExprStmt.second);
            sprIt->use.insert(currentExprStmt.second);
            // ひとつにまとめます。もし他のもののエイリアスであるなら、もう一方を更新します。
            //Union things together. If this was an alias of anoter thing, update the other thing
            if (sprIt->potentialAlias && !dereferenced) {
                if (!sprIt->aliases.empty()) {
                    // 問題は最後のエイリアスがイテレータであり他の関数で参照できてしまうことです。
                    // おそらくポインタに加工します。なぜ必要か分かりました。
                    //problem  because last alias is an iterator and can reference things in other functions.
                    //Maybe make into a pointer. Figure out why I need it.
                    auto spaIt = FunctionIt->second.find(*(sprIt->lastInsertedAlias));
                    if (spaIt != FunctionIt->second.end()) {
                        this->_logger->debug("dvars#4: {}", lhs->variableName);
                        spaIt->second.dvars.insert(lhs->variableName);
                        this->_logger->debug("use#5: {}", currentExprStmt.second);
                        spaIt->second.use.insert(currentExprStmt.second);
                        spaIt->second.slines.insert(currentExprStmt.second);
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
    for (NameLineNumberPair pair : useExprStack) {
        SliceProfile *useProfile = Find(pair.first);
        if (useProfile) {
            // 他の2つの式文の関数と同様同じ語に対して実行しています。
            //it's running on the same word as the other two exprstmt functions
            // use{} に追加
            this->_logger->debug("use#6: {}", pair.second);
            useProfile->use.insert(pair.second);
        }
    }
}

/**
 * ProcessDeclCtor
 * 宣言コンストラクタ?を処理します。
 */
void srcSliceHandler::ProcessDeclCtor() {
    SliceProfile *lhs = Find(currentDecl.first);
    if (!lhs) {
        return;
    } else {
        this->_logger->debug("use#7: {}", currentDecl.second);
        lhs->use.insert(currentDecl.second);
        SliceProfile *rhs = Find(currentDeclCtor.first);
        if (rhs) {
            this->_logger->debug("dvars#5: {}", lhs->variableName);
            rhs->dvars.insert(lhs->variableName);
            this->_logger->debug("use#8: {}", currentDecl.second);
            rhs->use.insert(currentDecl.second);
        }
    }
}

/**
 * ConputeInterprocedural
 * よく分からない。
 */
/**
 * ComputeInterprocedural
 * @param f File name
 * No return value
 */
void srcSliceHandler::ComputeInterprocedural(const std::string &f) {
    FileIt = sysDict->ffvMap.find(f);
    if (FileIt == sysDict->ffvMap.end()) {
        std::cerr << "CAN'T FIND FILE" << std::endl;
        return;
    } else {
        FunctionIt = (FileIt)->second.begin();
        FunctionVarMap::iterator FunctionItEnd = FileIt->second.end();

        for (; FunctionIt != FunctionItEnd; ++FunctionIt) {
            for (VarMap::iterator it = FunctionIt->second.begin(); it != FunctionIt->second.end(); ++it) {
                if (!it->second.visited) {
                    //std::unordered_set<NameLineNumberPair, NameLineNumberPairHash>::iterator - auto
                    for (auto itCF = it->second.cfunctions.begin(); itCF != it->second.cfunctions.end(); ++itCF) {
                        unsigned int argumentIndex = itCF->second;
                        SliceProfile Spi = ArgumentProfile(itCF->first, argumentIndex, it);
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
    auto gFuncIt = sysDict->fileFunctionTable.find(fname);
    if (gFuncIt != sysDict->fileFunctionTable.end()) {
        FileIt = sysDict->ffvMap.find(gFuncIt->second.fileName);
    }
    auto funcIt = FileIt->second.find(fname);
    if (funcIt != FileIt->second.end()) {
        VarMap::iterator v = funcIt->second.begin();
        for (auto it = v; it != funcIt->second.end(); ++it) {
            if (it->second.index == (parameterIndex)) {
                if (it->second.visited) {
                    if (it->second.potentialAlias) {
                        it->second.aliases.insert(vIt->second.variableName);
                    }
                    return it->second;
                } else {
                    for (auto itCF = it->second.cfunctions.begin(); itCF != it->second.cfunctions.end(); ++itCF) {
                        std::string newFunctionName = itCF->first;
                        unsigned int newParameterIndex = itCF->second;
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
