/**
 * @file srcSliceHandler.hpp
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

#include <srcSAXHandler.hpp>
#include <core/SliceProfile.hpp>
#include <helpers/Utility.hpp>
#include <core/srcSlice.hpp>
#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <sstream>
#include <stack>
#include <functional>
#include <spdlog/spdlog.h>
#include "types/srcSliceTypes.h"

class srcSliceHandler : public srcSAXHandler {
private:
    /*ParserState is a set of enums corresponding to srcML tags. Primarily, they're for addressing into the 
     *triggerField vector and figuring out which tags have been seen. It keeps a count of how many of each
     *tag is currently open. Increments at a start tag and decrements at an end tag*/
    /**
     * `ParserState は srcML のタグに相当する enum です。
     */
    enum ParserState {
        decl, expr, param, decl_stmt, expr_stmt, parameter_list,
        argument_list, argument_list_template, condition, call, templates, ctrlflow, endflow,
        name, function, functiondecl, constructor, constructordecl, destructordecl, destructor,
        argument, return_stmt, control, index, block, type, init, op, literal, modifier, member_list, classn,
        preproc, whileloop, forloop, ifcond, nonterminal, macro, classblock, functionblock,
        specifier, empty, MAXENUMVALUE = empty
    };

    std::unordered_map<std::string, std::function<void()>> process_map;
    std::unordered_map<std::string, std::function<void()>> process_map3;

    /**
     * 関数呼び出しでの引数のインデックス(one-based)を保持する
     */
    unsigned int argIndex;

    unsigned int declIndex;

    int constructorNum;

    SliceProfile currentSliceProfile;

    std::string fileName;

    std::string lhsName;
    unsigned int lhsLine;

    /**
     * パースを行う中で関数のデータを保持するために使用
     * Holds data for functions as we parse.
     */
    FunctionData functionTmplt;

    /*
     * どの関数が呼び出されたかをこのスタックに保持します。
     * 引数のSliceProfileを更新する必要があるときに役立ちます。
     * keeps track of which function has been called. Useful for when argument slice profiles need to be updated
     */
    std::stack<std::string> nameOfCurrentCalledFunctionStack;

    //Not used yet
    std::stack<unsigned int> controlFlowLineNum;

    /*These three iterators keep track of where we are inside of the system dictionary. They're primarily so that
     *there's no need to do any nasty map.finds on the dictionary (since it's a nested map of maps). These must
     *be updated as the file is parsed*/
    std::unordered_map<std::string, ClassProfile>::iterator classIt;
    FunctionVarMap *p_functionVarMap;
    VarMap *p_varMap;
    SliceProfile *p_sliceProfile;

    bool isConstructor;


    /**
     * 関数内にいないこと(つまりグローバルスコープ内)を示すフラグです
     * bool to tell us when we're not in a function
     */
    bool inGlobalScope;
    bool isACallName;

    //for decl_stmts
    bool opassign;
    bool sawnew;
    bool sawinit;
    bool sawgeneric;
    bool memberAccess;

    //for expr_stmts    
    bool expr_assign_flag;
    bool expr_op_flag;
    bool foundexprlhs;

    //For for loop init
    bool inFor;


    /**
     * これはなに？
     */
    std::list<NameAndLineNumber> useExprStack;

    bool potentialAlias;

    bool dereferenced;

    std::string calledFunctionName;
    std::string currentOperator;

    /**
     * 関数呼び出し時の引数を保持するスタック
     */
    std::stack<NameAndLineNumber> callArgDataStack;

    /*NameLineNumberPairs and triggerfield make up the meat of this slicer; check the triggerfield for context (E.g., triggerField[init])
     *and then once you know the right tags are open, check the correct line/string pair to see what the name is
     *at that position and its line number to be stored in the slice profile*/
    std::vector<unsigned short int> triggerField;
    NameAndLineNumber currentCallArgData;
    NameAndLineNumber currentParam;
    NameAndLineNumber currentParamType;

    NameAndLineNumber currentFunctionReturnType;

    NameAndLineNumber currentDecl;

    /**
     * extern が入ったりする
     */
    NameAndLineNumber currentDeclSpecifier;

    /**
     * const が入ったりする
     */
    NameAndLineNumber currentTypeSpecifier;
    NameAndLineNumber currentDeclType;

    /**
     * 宣言の初期化?
     */
    NameAndLineNumber currentDeclInit;
    NameAndLineNumber currentDeclCtor;

    NameAndLineNumber currentExprStmt;
    NameAndLineNumber lhsExprStmt;

    NameAndLineNumber useExprStmt;

    NameAndLineNumber currentDeclArg;
    NameAndLineNumber currentClassName;

    NameAndLineNumber currentFunctionBody;
    NameAndLineNumber currentFunctionDecl;

    // 制御構造を保持する
    std::stack<ControlData> control_data_stack;

    /*function headers*/
    void GetCallData();

    void GetParamName();

    void GetParamType();

    void ProcessDeclStmt();

    void GetFunctionData();

    void GetDeclStmtData();

    void ProcessDeclCtor();

    void AssignProfile();

    void ProcessExprStmtPreAssign();

    void ProcessExprStmtPostAssign();

    void ProcessExprStmtNoAssign();

    void ProcessConstructorDecl();

    void GetFunctionDeclData();

    SliceProfile createArgumentSp(std::string, ArgIndexAndMemberName);

    SliceProfile *Find(std::string varName);

    /**
     * triggerFieldOr, triggerFiledAnd
     * triggerFiled の or, and条件を表現する
     * C++11の可変長テンプレートを使った可変長引数関数を使っている
     */

    inline bool triggerFieldOr() {
        return false;
    }

    template<typename Head, typename... Rest>
    bool triggerFieldOr(Head first, Rest ... rest) {
        return triggerField[first] || triggerFieldOr(rest...);
    }

    inline bool triggerFieldAnd() {
        return true;
    }

    template<typename Head, typename ...Rest>
    bool triggerFieldAnd(Head first, Rest ...rest) {
        return triggerField[first] && triggerFieldAnd(rest...);
    }

    /**
     * パース中にセットされるファイルパスや関数名をもとに
     * 関数IDを取得します
     * @param lineNumber
     * @return
     */
    std::string getFunctionId(unsigned int lineNumber);


    /**
     * 変数IDを取得します
     * @param variableName
     * @return
     */
    std::string getVariableId(std::string variableName);

//    void insertDef(SliceProfile *sp, unsigned int lineNumber);

    void insertDef(SliceProfile *sp, unsigned int lineNumber, std::string name);

//    void insertUse(SliceProfile *sp, unsigned int lineNumber);

    void insertUse(SliceProfile *sp, unsigned int lineNumber, std::string name);

    void insertDvar(SliceProfile *sp, std::string variableName);

    /**
     * cfuncsに追加します
     * @param sp SliceProfile
     * @param calledFun 呼び出された関数名
     * @param argIndex 何番目の引数として渡されたか
     */
    void
    insertCFunc(SliceProfile *sp, std::string calledFunctionName, ArgIndexAndMemberName argIndex,
                unsigned int lineNumber);

    std::shared_ptr<spdlog::logger> _logger;

    void ComputeInterprocedural(const std::string &);

    void compute(SliceProfile &sp);

public:

    SliceDictionary *sysDict;
    unsigned int lineNum;

    srcSliceHandler(SliceDictionary *dict) {
        // スライス情報を格納する辞書への参照を得る
        sysDict = dict;

        // ロガーを初期化する
        this->_logger = spdlog::get("console")
                        ? spdlog::get("console")
                        : spdlog::stdout_logger_mt("console");
#ifdef DEBUG
        spdlog::set_level(spdlog::level::debug);
#else
        spdlog::set_level(spdlog::level::err);
#endif

        argIndex = 0;
        declIndex = 0;

        constructorNum = 0;
        lineNum = 0;
        potentialAlias = false;

        dereferenced = false;
        opassign = false;
        sawnew = false;
        sawinit = false;
        sawgeneric = false;
        memberAccess = false;

        expr_assign_flag = false;
        expr_op_flag = false;
        foundexprlhs = false;

        inFor = false;

        isACallName = false;
        isConstructor = false;
        inGlobalScope = true;
        triggerField = std::vector<unsigned short int>(MAXENUMVALUE, 0);
        process_map = {
                {"decl_stmt",        [this]() {
                    ++declIndex; //to keep track of index of declarations
                    ++triggerField[decl_stmt];
                }},

                {"expr_stmt",        [this]() {
                    ++triggerField[expr_stmt];
                }},
                {"parameter_list",   [this]() {
                    if (triggerFieldOr(function, functiondecl, constructor)
                        && !triggerFieldOr(functionblock, parameter_list, macro)) {
                        GetFunctionData();
                    }
                    if (triggerField[function]
                        && !triggerFieldOr(functionblock, type, parameter_list)) {
                        p_varMap = sysDict->variableTable.addFunction(fileName, functionTmplt.functionName);
                    }
                    if (triggerField[constructordecl]) { //For the case where we need to get a constructor decl
                        ProcessConstructorDecl();
                        currentDeclArg.name.clear();
                    }
                    if (triggerField[functiondecl]) {
                        functionTmplt.functionName = currentFunctionDecl.name;
                        currentFunctionDecl.name.clear();
                        if (triggerField[parameter_list] && triggerField[param] && triggerField[decl] &&
                            triggerField[type]) {
                            GetFunctionDeclData();
                        }
                    }
                    ++triggerField[parameter_list];
                }},

                {"if",               [this]() {
                    ++triggerField[ifcond];

                    ControlData cd(fileName, lineNum);
                    cd.controlRange.startLine = lineNum;

                    // if に入る時なのでスタックに入れる
                    control_data_stack.push(cd);
                    //controlFlowLineNum.push(lineNum);
                }},

                {"else",             [this]() {
                    control_data_stack.top().controlRange.elseLine = lineNum;
                }},

                {"for",              [this]() {
                    ++triggerField[forloop];
                    inFor = true;
                    //controlFlowLineNum.push(lineNum);
                }},

                {"while",            [this]() {
                    ++triggerField[whileloop];
                    //controlFlowLineNum.push(lineNum);
                }},

                {"condition",        [this]() {
                    //This gets used with expr_stmts since conditionals can basically contain everything an expr_stmt can contain
                    //This means that rules for expr_stmt usually have || condition next to them to re-use those same functions for
                    //things like while loops. If this becomes too big of a deal, might just write separate functions for conditions
                    ++triggerField[condition];
                }},

                {"argument_list",    [this]() {
                    ++triggerField[argument_list];
                    if (triggerField[call]) {
                        if (isACallName) {
                            isACallName = false;
                            nameOfCurrentCalledFunctionStack.push(calledFunctionName);
                            // すべてのグローバル変数について
                            for (auto &gv: *this->sysDict->variableTable.getRawGlobalVariableTable()) {
                                auto &gv_sp = gv.second;
                                // その関数内でdef/useまたは両方をするものは
                                // 関数呼び出し位置を追加する (See #32)
                                auto called_func_data = this->sysDict->functionTable.findByName(calledFunctionName);
                                if (called_func_data) {
                                    std::string called_func_id = called_func_data->computeId();
                                    ProgramPoint pp(lineNum, this->getFunctionId(lineNum));
                                    {
                                        for (auto &def : gv_sp.def) {
                                            if (def.programPoint.functionId == called_func_id) {
                                                DefUseData dd;
                                                dd.programPoint = pp;
                                                dd.derived_from_func_id = called_func_id;
                                                dd.member_name = def.member_name;
                                                gv_sp.def.insert(dd);
                                            }
                                        }
                                    }
                                    {
                                        for (auto &use: gv_sp.use) {
                                            if (use.programPoint.functionId == called_func_id) {
                                                DefUseData dd;
                                                dd.programPoint = pp;
                                                dd.derived_from_func_id = called_func_id;
                                                dd.member_name = use.member_name;
                                                gv_sp.use.insert(dd);
                                            }
                                        }
                                    }
                                }
                            }
                            calledFunctionName.clear();
                        }
                    }
                }},

                {"call",             [this]() {
                    if (triggerField[call]) {//for nested calls
                        --argIndex; //already in some sort of a call. Decrement counter to make up for the argument slot the function call took up.
                    }
                    isACallName = true;
                    ++triggerField[call];
                }},

                {"function",         [this]() {
                    inGlobalScope = false;
                    functionTmplt.fileName = fileName;
                    ++triggerField[function];
                }},
                {"constructor",      [this]() {
                    ++constructorNum;//constructors have numbers appended to them since they all have the same name.

                    isConstructor = true;
                    inGlobalScope = false;

                    ++triggerField[function];
                }},
                {"function_decl",    [this]() {
                    currentFunctionDecl.name.clear();
                    ++triggerField[functiondecl];
                }},
                {"destructor_decl",  [this]() {
                    currentFunctionDecl.name.clear();
                    ++triggerField[destructordecl];
                }},
                {"constructor_decl", [this]() {
                    currentFunctionDecl.name.clear();
                    ++triggerField[constructordecl];
                }},
                {"template",         [this]() {
                    ++triggerField[templates];
                }},
                {"class",            [this]() {
                    ++triggerField[classn];
                }},

                {"destructor",       [this]() {
                    inGlobalScope = false;
                    ++triggerField[function];
                }},
                {"parameter",        [this]() {
                    ++triggerField[param];
                    ++declIndex;
                }},
                {"member_list",      [this]() {
                    ++triggerField[member_list];
                }},
                {"return",           [this]() {
                    ++triggerField[return_stmt];
                }},
                {"control",          [this]() {
                    ++triggerField[control];
                }},
                {"index",            [this]() {
                    ++triggerField[index];
                }},
                {"operator",         [this]() {
                    ++triggerField[op];
                    if (triggerField[expr_stmt]) {
                        expr_op_flag = true; //assume we're not seeing =
                    }
                    if (triggerField[return_stmt]) {
                        //ProcessExprStmtPreAssign(); //To catch expressions in return statements that are split by operators
                        lhsExprStmt.name.clear();
                    }

                }},
                {"block",            [this]() {
                    if (triggerFieldOr(function, constructor)) {
                        ++triggerField[functionblock];
                    }
                    if (triggerField[classn]) {
                        ++triggerField[classblock];
                    }
                    ++triggerField[block];
                }},
                {"init",             [this]() {
                    //This one is only called if we see init. If there's no init, it's safely ignored.
                    if (triggerField[decl_stmt] /* && triggerFieldOr(constructor, function) */ ) {
                        GetDeclStmtData();
                        sawinit = true;
                    }
                    memberAccess = false;
                    currentDecl.name.clear();
                    ++triggerField[init];
                }},
                {"argument",         [this]() {
                    ++argIndex;
                    currentCallArgData.name.clear();
                    calledFunctionName.clear();
                    ++triggerField[argument];
                }},
                {"literal",          [this]() {
                    ++triggerField[literal];
                }},
                {"modifier",         [this]() {
                    ++triggerField[modifier];
                }},
                {"decl",             [this]() {
                    ++triggerField[decl];
                }},
                {"type",             [this]() {
                    ++triggerField[type];
                }},
                {"expr",             [this]() {
                    ++triggerField[expr];
                }},
                {"name",             [this]() {
                    ++triggerField[name];
                    useExprStmt.lineNumber
                            = lhsExprStmt.lineNumber
                            = currentCallArgData.lineNumber
                            = currentParam.lineNumber
                            = currentParamType.lineNumber
                            = currentFunctionBody.lineNumber
                            = currentDecl.lineNumber
                            = currentExprStmt.lineNumber
                            = currentFunctionDecl.lineNumber
                            = currentDeclInit.lineNumber
                            = lineNum;
                    // <function><type><name>とタグが続いた時で
                    // まだ開始行がセットされていない時だけ行番号を設定する
                    if (triggerFieldAnd(function, type, name)
                        && !functionTmplt.declareRange.isStartLineSet()) {
                        functionTmplt.declareRange.startLine = lineNum;
                    }
                }},
                {"macro",            [this]() {
                    ++triggerField[macro];
                }},
                {"specifier",        [this]() {
                    ++triggerField[specifier];
                }}
        };
        process_map3 = {

                {"decl_stmt",        [this]() {
                    currentCallArgData.name.clear();
                    currentDeclArg.name.clear();
                    currentDeclCtor.name.clear();
                    potentialAlias = false;
                    sawinit = false;
                    currentDeclInit.name.clear();
                    currentDeclSpecifier.name.clear();
                    declIndex = 0;
                    --triggerField[decl_stmt];
                }},

                {"expr_stmt",        [this]() {
                    --triggerField[expr_stmt];

                    //for decl_stmts
                    opassign = false;
                    dereferenced = false;

                    //for expr_stmts
                    foundexprlhs = false;
                    expr_op_flag = false;
                    expr_assign_flag = false;

                    // collect data about things that were not in assignment expr_stmts
                    // #16のためコメントアウト
                    // ProcessExprStmtNoAssign();

                    lhsLine = 0;
                    useExprStack.clear(); //clear data
                    lhsName.clear();
                    currentExprStmt.name.clear();
                    lhsExprStmt.name.clear();
                    useExprStmt.name.clear();
                    currentCallArgData.name.clear();
                }},

                {"parameter_list",   [this]() {
                    --triggerField[parameter_list];
                }},

                {"if",               [this]() {
                    --triggerField[ifcond];
                    auto a = control_data_stack.top();
                    control_data_stack.pop();
                    a.controlRange.endLine = lineNum;
                    sysDict->controlTable.add(a);
                }},

                {"for",              [this]() {
                    --triggerField[forloop];
                }},

                {"while",            [this]() {
                    --triggerField[whileloop];
                }},

                {"condition",        [this]() {
                    --triggerField[condition];
                    //for expr_stmts
                    foundexprlhs = false;
                    expr_op_flag = false;
                    expr_assign_flag = false;

                    //collect data about things that were not in assignment expr_stmts
                    ProcessExprStmtNoAssign();
                    useExprStack.clear(); //clear data

                    //uncategorized
                    lhsLine = 0;
                    lhsName.clear();
                    currentExprStmt.name.clear();
                    lhsExprStmt.name.clear();

                    useExprStmt.name.clear();
                }},

                {"argument_list",    [this]() {
                    if (triggerFieldAnd(decl, decl_stmt) && (!triggerField[init] || triggerFieldOr(argument, macro))) {
                        GetDeclStmtData();
                    }
                    argIndex = 0;
                    sawgeneric = false;
                    calledFunctionName.clear();
                    --triggerField[argument_list];
                }},

                {"call",             [this]() {
                    if (!nameOfCurrentCalledFunctionStack.empty()) {
                        nameOfCurrentCalledFunctionStack.pop();
                    }
                    --triggerField[call];
                    if (triggerField[call]) {
                        ++argIndex; //we exited a call but we're still in another call. Increment to make up for decrementing when we entered the second call.
                    }
                }},

                {"function",         [this]() {
                    declIndex = 0;
                    inGlobalScope = true;

                    // 関数の終了行を設定し、関数を辞書に登録する
                    functionTmplt.declareRange.endLine = lineNum;
                    sysDict->functionTable.add(functionTmplt.functionName, functionTmplt);

                    functionTmplt.clear();

                    if (triggerField[function] &&
                        !triggerFieldOr(functionblock, templates, parameter_list, type, argument_list)) {
                        currentFunctionBody.name.clear();
                    }

                    --triggerField[function];
                }},
                {"constructor",      [this]() {
                    isConstructor = false;
                    declIndex = 0;

                    inGlobalScope = true;
                    functionTmplt.clear();
                    --triggerField[function];
                }},

                {"destructor",       [this]() {
                    declIndex = 0;

                    inGlobalScope = true;
                    functionTmplt.clear();
                    --triggerField[function];
                }},
                {"function_decl",    [this]() {
                    currentFunctionDecl.name.clear();
                    --triggerField[functiondecl];
                }},
                {"constructor_decl", [this]() {
                    currentFunctionDecl.name.clear();
                    --triggerField[functiondecl];
                }},
                {"destructor_decl",  [this]() {
                    currentFunctionDecl.name.clear();
                    --triggerField[functiondecl];
                }},
                {"class",            [this]() {
                    currentClassName.name.clear();
                    --triggerField[classn];
                }},
                {"parameter",        [this]() {
                    if (triggerFieldAnd(parameter_list, param) && !triggerFieldOr(type, functionblock, templates)) {
                        GetParamName();
                    }
                    potentialAlias = false;
                    --triggerField[param];
                }},
                {"member_list",      [this]() {
                    --triggerField[member_list];
                }},
                {"return",           [this]() {
                    ProcessExprStmtNoAssign();
                    useExprStack.clear();//to catch expressions in return statements
                    --triggerField[return_stmt];
                }},
                {"control",          [this]() {
                    inFor = false;
                    --triggerField[control];
                }},
                {"index",            [this]() {
                    --triggerField[index];
                }},
                {"operator",         [this]() {
                    calledFunctionName.clear();
                    if (triggerField[decl_stmt] && (currentOperator == "." || currentOperator == "->")) {
                        memberAccess = true;
                    }
                    if (currentDeclInit.name == "new") {
                        //separate new operator because we kinda need to know when we see it.
                        sawnew = true;
                    }
                    //The logic for exprassign and op is basically that we want to know when we've hit the left and right hand side of expr stmt
                    //expr assign is set when we see =. Everything read up to that point is lhs. exprop is any other operator. When I see that
                    //I know that we're probably in a member call chain a->b->c etc. I don't care about b and c, so expr op helps skip those.
                    if (triggerFieldOr(expr_stmt, condition)) {

//                        useExprStmt.name.clear();

                        if (expr_assign_flag) {
                            ProcessExprStmtPreAssign();
                            if (!lhsExprStmt.name.empty()) {
                                lhsName = lhsExprStmt.name;
                            }
                            lhsExprStmt.name.clear();
                            expr_op_flag = false;
                            //foundexprlhs = true;
                        }
                    }
                    if (!(sawgeneric || currentDeclCtor.name.empty())
                        && triggerFieldAnd(decl_stmt, argument)
                        && !triggerField[init]) {
                        //std::cerr<<"Call from op"<<std::endl;
                        //GetDeclStmtData();
                        ProcessDeclCtor(); //-- to process decl_stmts that use constructor syntax
                        sawinit = true;
                        currentDeclCtor.name.clear();
                    }

//                    currentDeclInit.name.clear();
                    currentOperator.clear();

                    --triggerField[op];
                }},
                {"block",            [this]() {
                    if (triggerFieldOr(function, constructor)) {
                        --triggerField[functionblock];
                    }
                    if (triggerField[classn]) {
                        --triggerField[classblock];
                    }
                    --triggerField[block];
                }},
                {"init",             [this]() {//so that we can get more stuff after the decl's name
                    --triggerField[init];
                }},
                {"argument",         [this]() {
                    currentDeclArg.name.clear(); //get rid of the name of the var that came before it for ctor calls like: object(InitVarable)
                    currentCallArgData.name.clear();
                    calledFunctionName.clear();
                    if (!(sawgeneric || currentDeclCtor.name.empty())
                        && triggerFieldAnd(decl_stmt, argument)
                        && !triggerField[init]) {
                        //std::cerr<<"Call from arg"<<std::endl;
                        //GetDeclStmtData();
                        ProcessDeclCtor(); //-- to process decl_stmts that use constructor syntax
                        sawinit = true;
                        currentDeclCtor.name.clear();
                    }
                    --triggerField[argument];
                }},
                {"literal",          [this]() {
                    --triggerField[literal];
                }},
                {"modifier",         [this]() {
                    if (triggerFieldAnd(decl_stmt, decl)) { //only care about modifiers in decls
                        potentialAlias = true;
                    } else if (triggerFieldAnd(function, parameter_list, param, decl)) {
                        potentialAlias = true;
                    }
                    --triggerField[modifier];
                }},
                {"template",         [this]() {
                    --triggerField[templates];
                }},
                {"decl",             [this]() {
                    if (!sawinit && triggerField[decl_stmt] /* && triggerFieldOr(constructor, function) */) {
                        //only run if we didn't run it during init
                        GetDeclStmtData();
                    }
                    currentDecl.name.clear();
                    --triggerField[decl];
                }},
                {"type",             [this]() {
                    if (triggerFieldAnd(type, decl_stmt)
                        // && triggerFieldOr(function, constructor) // この条件があるとグローバル変数の型名がセットされない
                        && !triggerFieldOr(modifier, argument_list)) {
                        //Get the type -- news
                        currentSliceProfile.variableType = currentDeclType.name;
                        currentDeclType.name.clear();
                        currentTypeSpecifier.name.clear();
                    }
                    if (triggerFieldAnd(type, parameter_list, param, decl) &&
                        !triggerFieldOr(functionblock, templates)) {
                        GetParamType();
                    }
                    if (triggerFieldAnd(function, type)
                        && !triggerFieldOr(functionblock, argument_list, templates, parameter_list)) {
                        //get functionr ret type  -- news
                        functionTmplt.returnType = currentFunctionReturnType.name;
                        currentFunctionReturnType.name.clear();
                    }
                    if (triggerFieldAnd(functiondecl, type)
                        && !triggerFieldOr(parameter_list, member_list)) {
                        //get function decl return type
                        functionTmplt.returnType = currentFunctionDecl.name;
                    }
                    --triggerField[type];
                }},
                {"expr",             [this]() {
                    --triggerField[expr];
                }},
                {"name",             [this]() {
                    if (triggerField[name] == 1 && triggerField[call] && triggerField[argument]) {
                        callArgDataStack.push(currentCallArgData);
                    }
                    //Get function arguments
                    if (triggerField[name] == 1 &&
                        (triggerField[call] || triggerFieldAnd(decl_stmt, argument_list))) {
                        GetCallData();//issue with statements like object(var)
                        while (!callArgDataStack.empty())
                            callArgDataStack.pop();
                    }
                    if (triggerField[expr]
                        && triggerFieldOr(expr_stmt, condition, return_stmt)) {
                        if (expr_assign_flag && triggerField[name] < 2) {
                            ProcessExprStmtPostAssign();
                            useExprStack.clear(); //found an assignment so throw everything off of the other stack TODO: Potential better way?
                            currentExprStmt.name.clear();
                        } else {
                            if (!useExprStmt.name.empty()) {
                                if (triggerField[name] < 2) {
                                    if (Find(useExprStmt.name)) {
                                        useExprStack.push_back(useExprStmt);
                                    }
                                    useExprStmt.name.clear();
                                }
                            }
                        }
                    }
                    if (triggerFieldAnd(init, decl)
                        && triggerFieldOr(decl_stmt, control)
                        && !triggerFieldOr(type, argument_list)) {
                        if (triggerField[name] == 1) {
                            ProcessDeclStmt();
                        }
                    }
                    memberAccess = false;
                    expr_op_flag = false; //reset expr after each name so that the next name will be read unless there's another op in front of it
                    --triggerField[name];
                }},
                {"macro",            [this]() {
                    currentDeclType.name.clear();
                    currentDecl.name.clear();
                    --triggerField[macro];
                }},
                {"specifier",        [this]() {
                    --triggerField[specifier];
                }},
        };

    }

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"


    /*
    virtual void startDocument() {}
    virtual void endDocument() {}
    */

    /**
     * startRoot
     * @param localname the name of the element tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     * @param nb_namespaces number of namespaces definitions
     * @param namespaces the defined namespaces
     * @param nb_attributes the number of attributes on the tag
     * @param nb_defaulted the number of defaulted attributes
     * @param attributes list of attribute name value pairs (localname/prefix/URI/value/end)
     *
     * SAX handler function for start of the root element.
     * Counts the root unit (if an archive, to avoid double count with startUnit).
     * Overide for desired behaviour.
     */
    virtual void startRoot(const char *localname, const char *prefix, const char *URI,
                           int num_namespaces, const struct srcsax_namespace *namespaces, int num_attributes,
                           const struct srcsax_attribute *attributes) {

        //push_element(localname, prefix);
    }

    /**
     * startUnit
     * @param localname the name of the element tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     * @param nb_namespaces number of namespaces definitions
     * @param namespaces the defined namespaces
     * @param nb_attributes the number of attributes on the tag
     * @param nb_defaulted the number of defaulted attributes
     * @param attributes list of attribute name value pairs (localname/prefix/URI/value/end)
     *
     * SAX handler function for start of an unit.
     * Counts each unit tag (= filecount non-archive, = filecount + 1 if archive).
     * Overide for desired behaviour.
     */
    virtual void startUnit(const char *localname, const char *prefix, const char *URI,
                           int num_namespaces, const struct srcsax_namespace *namespaces, int num_attributes,
                           const struct srcsax_attribute *attributes) {
        //fileNumber = functionNameHash(attributes[1].value);
        fileName = std::string(attributes[2].value);
        p_functionVarMap = sysDict->variableTable.addFile(fileName);
        // GLOBALスコープのものはここで作られる
        // NOTE: 実際の出力にGLOBALは出てこないものの、コメントアウトするとなぜか
        // 実行時にエラーになるので復活させた
        p_varMap = &p_functionVarMap->insert(std::make_pair("GLOBAL", VarMap())).first->second;
    }

    /**
     * startElementNs
     * @param localname the name of the element tag
     * @param prefix the tag prefix
     * @param URI the namespace of tag
     * @param nb_namespaces number of namespaces definitions
     * @param namespaces the defined namespaces
     * @param nb_attributes the number of attributes on the tag
     * @param nb_defaulted the number of defaulted attributes
     * @param attributes list of attribute name value pairs (localname/prefix/URI/value/end)
     *
     * SAX handler function for start of an element.
     * Overide for desired behaviour.
    */
    virtual void startElement(
            const char *localname,
            const char *prefix,
            const char *URI,
            int num_namespaces,
            const struct srcsax_namespace *namespaces,
            int num_attributes,
            const struct srcsax_attribute *attributes) {
        std::string name;
        if (num_attributes) {
            // ここで行番号を取っている
            // 単純に条件をtrueにするだけではだめだった
            // ここをうまく解決しよう
            lineNum = strtoul(attributes[0].value, NULL, 0);
            name = attributes[0].value;
        }
        std::string lname(localname);
        std::string lnspace;
        if (prefix) {
            lnspace.append(prefix);
        }
        if (lnspace == "cpp") {
            ++triggerField[preproc];
        }
        if (name == "generic") {
            sawgeneric = true;
        }

        std::unordered_map<std::string, std::function<void()>>::const_iterator process = process_map.find(lname);
        if (process != process_map.end()) {
            process->second();
        }
    }

    /**
     * charactersUnit
     * @param ch the characers
     * @param len number of characters
     *
     * SAX handler function for character handling within a unit.
     * Overide for desired behaviour.
     */
    virtual void charactersUnit(const char *ch, int len) {

        if (triggerFieldOr(decl_stmt, expr_stmt) &&
            triggerFieldAnd(call, name, argument_list) &&
            !triggerFieldOr(index, preproc)) {
            std::string s(ch, len);
            currentCallArgData.name.append(s);
        }
        if (triggerFieldOr(function, constructor, destructor) && triggerField[name] &&
            !triggerFieldOr(functionblock, parameter_list, argument_list, argument_list_template, type, index, preproc,
                            op, macro)) {
            currentFunctionBody.name.append(ch, len);
            //std::cerr<<currentFunctionBody.first<<std::endl;
        }
        if (triggerFieldAnd(type, function)
            &&
            !triggerFieldAnd(functionblock, op, argument_list, argument_list_template, templates, parameter_list, macro,
                             preproc)) {

            if (!triggerField[modifier]) {
                currentFunctionReturnType.name = std::string(ch, len);
            } else {
                currentFunctionReturnType.name.append(ch, len);
            }
        }
        if (triggerFieldOr(functiondecl, constructordecl) && triggerField[name] &&
            !triggerFieldOr(functionblock, parameter_list, argument_list, argument_list_template, type, index, preproc,
                            op, macro)) {
            currentFunctionDecl.name.append(ch, len);
        }
        if (triggerField[param]
            && triggerFieldOr(function, functiondecl, constructor)
            && triggerField[name]
            && !triggerFieldOr(type, argument_list, templates, macro, preproc)) {
            currentParam.name.append(ch, len);
        }
        if (triggerField[param]
            && triggerFieldOr(function, functiondecl, constructor)
            && triggerFieldAnd(name, type)
            && !(triggerFieldOr(argument_list, templates, op, macro, preproc))) {
            currentParamType.name = currentTypeSpecifier.name.empty()
                                    ? std::string(ch, len)
                                    : currentTypeSpecifier.name + " " + std::string(ch, len);
        }
        if (triggerFieldAnd(type, decl_stmt, name) &&
            !triggerFieldOr(argument_list, modifier, op, macro, preproc)) {
            currentDeclType.name = currentTypeSpecifier.name.empty()
                                   ? std::string(ch, len)
                                   : currentTypeSpecifier.name + " " + std::string(ch, len);
        }
        if (triggerFieldAnd(decl, type, specifier)
                ) {
            // const な変数の時、specifierがconstになるので補足する
            currentTypeSpecifier.name = std::string(ch, len);
        }
        if (triggerFieldAnd(decl_stmt, decl, specifier)) {
            // extern になったりするので補足する
            currentDeclSpecifier.name = std::string(ch, len);
        }
        //this is to handle lhs of decl stmts and rhs of decl stmts
        if (triggerFieldOr(name, op)
            && triggerFieldOr(decl_stmt, control)
            && triggerField[decl]
            && !triggerFieldOr(argument_list, preproc, type, macro)) {
            std::string str(ch, len);

            if (triggerField[init]) {
                if (triggerField[op]) {
                    if (str == ".") {
                        currentDeclInit.name.append(str);

                    }
                } else {
                    currentDeclInit.name.append(str);

                }
            } else {
                if (!triggerField[op]) {
                    currentDecl.name.append(str);
                }
            }
        }
        //this is to handle lhs of decl stmts and rhs of decl stmts
        if (!sawgeneric &&
            triggerFieldAnd(name, decl_stmt, argument_list, decl) &&
            !triggerFieldOr(op, index, preproc, type, macro)) {
            currentDeclCtor.name.append(ch, len);
        }
        if (triggerField[op]) {
            currentOperator.append(ch, len);
        }
        // a = b; のような形式の代入文のみをここでは扱います
        // "="を含まないものはここでは処理しません
        // This only handles expr statments of the form a = b.
        // Anything without = in it is skipped here -- Not entirely true anymore
        if (triggerFieldOr(name, op)
            && triggerField[expr]
            && triggerFieldOr(expr_stmt, condition, return_stmt)
            && !triggerFieldOr(index, preproc)) {
            std::string str = std::string(ch, len);

            // ここでちょっとしたハックをしています。currentOperatorは
            // 基本的にそのオペレーターが代入文のものであるか、
            // または比較の<=であるかを区別するためのものです。
            // kind of a hack here; currentOperator basically tells
            // me if the operator was actually assignment or some
            // kinda compare operator like <=. Important to know
            // which one I just saw since I need to assign to use or def.

            auto lastChar = str.back();
            if (lastChar == '=' && currentOperator.size() < 2) {
                // '='は代入を行うものであり、
                // 比較の<=などではないというフラグを立てる
                expr_assign_flag = true;
                expr_op_flag = false;
                // assumed above in "operator" that I wouldn't see =.
                // This takes care of when I assume wrong.
                // don't read the =, just want to know it was there.
                str.clear();
            }
            if (triggerField[op]) {
                if (str == "*") {
                    // 最後に見た後は被参照されている
                    // most recent word was dereferenced
                    dereferenced = true;
                    //slight hack. Need to be able to tell when * is used as dereferenced because I don't wanna skip
                    expr_op_flag = false;
                }
                // 構造体メンバにアクセスする時はそのまま保持しておく
                if (str != ".") {
                    str.clear();
                }
            }
            // 代入が起きている時
            if (expr_assign_flag) {
                // どっちに分岐しても同じことをする
                // if it's not in a call then we can do like normal
                if (!triggerField[call]) {
                    currentExprStmt.name.append(str);
                } else {
                    currentExprStmt.name.append(str);
                }
            } else {
                if (!expr_op_flag) {
                    // '='を含むoperatorに出会っていないならば
                    //haven't seen any operator (including =)
                    lhsExprStmt.name = std::string(str);
                } else {
                    lhsExprStmt.name.append(str);
                    // useExprStack.push_back(strLine);
                }
                useExprStmt.name.append(str);
            }
        }
        if (triggerField[call]) {
            calledFunctionName.append(ch, len);
        }
        if (triggerFieldAnd(decl_stmt, decl, argument_list, argument, expr) &&
            !triggerField[type]) {
            currentDeclArg.name.append(ch, len);
        }
        if (!triggerField[classblock] && triggerFieldAnd(name, classn)) {
            currentClassName.name.append(ch, len);
        }
    }

    // end elements may need to be used if you want to collect only on per file basis or some other granularity.
    virtual void endRoot(const char *localname, const char *prefix, const char *URI) {

    }

    virtual void endUnit(const char *localname, const char *prefix, const char *URI) {
        fileName.clear();
    }

    virtual void endElement(const char *localname, const char *prefix, const char *URI) {
        std::string lname(localname);
        std::string lnspace;
        if (prefix) {
            lnspace.append(prefix);
        }
        if (lnspace == "cpp") {
            currentDeclType.name.clear();
            currentDecl.name.clear();
            --triggerField[preproc];
        }
        std::unordered_map<std::string, std::function<void()>>::const_iterator process3 = process_map3.find(lname);
        if (process3 != process_map3.end()) {
            process3->second();
        }
#pragma GCC diagnostic pop
    }

    /**
     * 手続き間の関係を計算します
     * @param srcSliceHandler
     * @param var_table
     */
    void computeAllInterproceduralRelation();
};
