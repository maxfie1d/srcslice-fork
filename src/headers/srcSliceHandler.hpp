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
#include <SliceProfile.hpp>
#include <Utility.hpp>
#include <srcSlice.hpp>
#include <iostream>
#include <vector>
#include <list>
#include <algorithm>
#include <sstream>
#include <stack>
#include <functional>
#include <spdlog/spdlog.h>
#include "srcSliceTypes.h"

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

    unsigned int numArgs;
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

    /*keeps track of which function has been called. Useful for when argument slice profiles need to be updated*/
    std::stack<std::string> nameOfCurrentClldFcn;

    //Not used yet
    std::stack<unsigned int> controlFlowLineNum;

    /*These three iterators keep track of where we are inside of the system dictionary. They're primarily so that
     *there's no need to do any nasty map.finds on the dictionary (since it's a nested map of maps). These must
     *be updated as the file is parsed*/
    std::unordered_map<std::string, ClassProfile>::iterator classIt;
    FileFunctionVarMap::iterator FileIt;
    FunctionVarMap::iterator FunctionVarMapItr;
    VarMap::iterator varIt;

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
    bool exprassign;
    bool exprop;
    bool foundexprlhs;

    //For for loop init
    bool inFor;
    std::list<NameAndLineNumber> useExprStack;

    bool potentialAlias;

    bool dereferenced;

    std::string calledFunctionName;
    std::string currentOperator;
    std::stack<NameAndLineNumber> callArgData;

    /*NameLineNumberPairs and triggerfield make up the meat of this slicer; check the triggerfield for context (E.g., triggerField[init])
     *and then once you know the right tags are open, check the correct line/string pair to see what the name is
     *at that position and its line number to be stored in the slice profile*/
    std::vector<unsigned short int> triggerField;
    NameAndLineNumber currentCallArgData;
    NameAndLineNumber currentParam;
    NameAndLineNumber currentParamType;

    NameAndLineNumber currentFunctionReturnType;

    NameAndLineNumber currentDecl;
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

    SliceProfile ArgumentProfile(std::string, unsigned int, VarMap::iterator);

    SliceProfile *Find(const std::string &varName);

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

    std::shared_ptr<spdlog::logger> _logger;

public:
    void ComputeInterprocedural(const std::string &);

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

        numArgs = 0;
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

        exprassign = false;
        exprop = false;
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
                        FunctionVarMapItr = FileIt->second.insert(
                                std::make_pair(functionTmplt.functionName, VarMap())).first;
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
                    //controlFlowLineNum.push(lineNum);
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
                            nameOfCurrentClldFcn.push(calledFunctionName);
                            calledFunctionName.clear();
                        }
                    }
                }},

                {"call",             [this]() {
                    if (triggerField[call]) {//for nested calls
                        --numArgs; //already in some sort of a call. Decrement counter to make up for the argument slot the function call took up.
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
                        exprop = true; //assume we're not seeing =
                    }
                    //Don't want the operators. But do make a caveat for ->
                    if (triggerField[call]) {
                        currentCallArgData.name.clear();
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
                    ++numArgs;
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
                    --triggerField[decl_stmt];
                }},

                {"expr_stmt",        [this]() {
                    --triggerField[expr_stmt];

                    //for decl_stmts
                    opassign = false;
                    dereferenced = false;

                    //for expr_stmts
                    foundexprlhs = false;
                    exprop = false;
                    exprassign = false;

                    ProcessExprStmtNoAssign(); //collect data about things that were not in assignment expr_stmts
                    useExprStack.clear(); //clear data

                    lhsLine = 0;
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
                    exprop = false;
                    exprassign = false;

                    ProcessExprStmtNoAssign(); //collect data about things that were not in assignment expr_stmts
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
                    numArgs = 0;
                    sawgeneric = false;
                    calledFunctionName.clear();
                    --triggerField[argument_list];
                }},

                {"call",             [this]() {
                    if (!nameOfCurrentClldFcn.empty()) {
                        nameOfCurrentClldFcn.pop();
                    }
                    --triggerField[call];
                    if (triggerField[call]) {
                        ++numArgs; //we exited a call but we're still in another call. Increment to make up for decrementing when we entered the second call.
                    }
                }},

                {"function",         [this]() {
                    declIndex = 0;
                    inGlobalScope = true;

                    // 関数の終了行を設定し、
                    // 関数を辞書に登録する
                    functionTmplt.declareRange.endLine = lineNum;
                    sysDict->fileFunctionTable.insert(std::make_pair(functionTmplt.functionName, functionTmplt));

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

                        useExprStmt.name.clear();

                        if (exprassign) {
                            ProcessExprStmtPreAssign();
                            if (!lhsExprStmt.name.empty()) {
                                lhsName = lhsExprStmt.name;
                            }
                            lhsExprStmt.name.clear();
                            exprop = false;
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

                    currentDeclInit.name.clear();
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
                    if (triggerField[call] && triggerField[argument]) {
                        callArgData.push(currentCallArgData);
                    }
                    //Get function arguments
                    if (triggerField[call] || triggerFieldAnd(decl_stmt, argument_list)) {
                        GetCallData();//issue with statements like object(var)
                        while (!callArgData.empty())
                            callArgData.pop();
                    }
                    if (triggerField[expr]
                        && triggerFieldOr(expr_stmt, condition, return_stmt)) {
                        if (exprassign) {
                            ProcessExprStmtPostAssign();
                            useExprStack.clear(); //found an assignment so throw everything off of the other stack TODO: Potential better way?
                            currentExprStmt.name.clear();
                        } else {
                            if (!useExprStmt.name.empty()) {
                                if (Find(useExprStmt.name)) {
                                    useExprStack.push_back(useExprStmt);
                                }
                                useExprStmt.name.clear();
                            }
                        }
                    }
                    if (triggerFieldAnd(init, decl)
                        && triggerFieldOr(decl_stmt, control)
                        && !triggerFieldOr(type, argument_list)) {
                        if (triggerField[call] && !memberAccess) {
                            ProcessDeclStmt();
                        } else if (!triggerField[call] && !memberAccess) {
                            ProcessDeclStmt();
                        }
                    }
                    memberAccess = false;
                    exprop = false; //reset expr after each name so that the next name will be read unless there's another op in front of it
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
        FileIt = sysDict->ffvMap.insert(
                std::make_pair(fileName, FunctionVarMap())).first; //insert and keep track of most recent.
        //std::cerr<<"val: "<<attributes[1].value<<std::endl;exit(1);
        //classIt = sysDict->classTable.insert(std::make_pair("GLOBAL", ClassProfile())).first;

        // GLOBALスコープのものはここで作られる
        // for globals. Makes a bad assumption about where globals are. Fix.
        // FunctionVarMapItr = FileIt->second.insert(std::make_pair("GLOBAL", VarMap())).first;
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
            !triggerFieldOr(index, op, preproc)) {
            currentCallArgData.name.append(ch, len);
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
        //this is to handle lhs of decl stmts and rhs of decl stmts
        if (triggerFieldOr(name, op)
            && triggerFieldOr(decl_stmt, control)
            && triggerField[decl]
            && !triggerFieldOr(argument_list,  preproc, type, macro)) {
            if (triggerField[init]) {
                if (!triggerField[call] && !memberAccess) {//if it's not in a call then we can do like normal
                    currentDeclInit.name.append(ch, len);
                } else {
                    if (!memberAccess) {
                        currentDeclInit.name.append(ch, len);
                    }
                    if (triggerField[argument]) {//if it's in a call, ignore until we hit an argument
                        currentDeclInit.name.append(ch, len);
                    }
                }
            } else {
                if (!triggerField[op]) {
                    currentDecl.name.append(ch, len);
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
        //This only handles expr statments of the form a = b. Anything without = in it is skipped here -- Not entirely true anymore
        if (triggerFieldOr(name, op) && triggerField[expr] &&
            triggerFieldOr(expr_stmt, condition, return_stmt) &&
            !triggerFieldOr(index, preproc)) {
            std::string str = std::string(ch, len);
            //std::cerr<<"str: "<<str<<currentExprStmt.second<<std::endl;
            //kind of a hack here; currentOperator basically tells me if the operator was actually assignment
            //or some kinda compare operator like <=. Important to know which one I just saw since I need to assign to use or def.
            // std::string::back() -> 末尾の文字を返す関数
            if (str.back() == '=' && currentOperator.size() < 2) {
                exprassign = true;
                exprop = false; //assumed above in "operator" that I wouldn't see =. This takes care of when I assume wrong.
                str.clear(); //don't read the =, just want to know it was there.
            }
            if (triggerField[op]) {
                if (str == "*") {
                    dereferenced = true; //most recent word was dereferenced
                    exprop = false; //slight hack. Need to be able to tell when * is used as dereferenced because I don't wanna skip
                }
                str.clear();
            }
            if (exprassign) {
                if (!triggerField[call]) {//if it's not in a call then we can do like normal
                    currentExprStmt.name.append(str);
                } else {
                    currentExprStmt.name.append(str);
                }
            } else {
                if (!exprop) { //haven't seen any operator (including =)
                    lhsExprStmt.name = std::string(str);
                    if (triggerField[return_stmt]) {
                        auto strLine = NameAndLineNumber(str, currentExprStmt.lineNumber);
                        useExprStack.push_back(strLine); //catch expr_stmts like return temp + temp;
                    }
                }
                useExprStmt.name.append(str); //catch expr_stmts like cout<<identifier;
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
};

inline void DoComputation(srcSliceHandler &h, const FileFunctionVarMap &mp) {
    for (FileFunctionVarMap::const_iterator ffvmIt = mp.begin(); ffvmIt != mp.end(); ++ffvmIt) {
        h.ComputeInterprocedural(ffvmIt->first);
    }
}
