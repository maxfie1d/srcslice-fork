/**
 * @file srcSlice.cpp
 *
 * @copyright Copyright (C) 2013-2014  SDML (www.srcML.org)
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
#include <json.hpp>

void TestSlice2(const VarMap &mp) {
    for (VarMap::const_iterator vmIt = mp.begin(); vmIt != mp.end(); ++vmIt) {
        std::cerr << "-------------------------" << std::endl;
        std::cerr << "Variable: " << vmIt->first << std::endl;
        std::cerr << "Slines: {";
        for (unsigned int sl : vmIt->second.slines) {
            std::cerr << sl << ",";
        }
        std::cerr << "}" << std::endl;
        std::cerr << "dvars: {";
        for (std::string dv : vmIt->second.dvars) {
            std::cerr << dv << ",";
        }
        std::cerr << "}" << std::endl;
        std::cerr << "is aliase for: {";
        for (std::string al : vmIt->second.aliases) {
            std::cerr << al << ",";
        }
        std::cerr << "}" << std::endl;
        std::cerr << "cfuntions: {";
        for (auto cfunc : vmIt->second.cfunctions) {
            std::cerr << cfunc.first << " " << cfunc.second << ",";
        }
        std::cerr << "}" << std::endl;
        std::cerr << "-------------------------" << std::endl;
    }
}

void TestSlice(const FileFunctionVarMap &mp, srcSliceHandler handler) {
    for (FileFunctionVarMap::const_iterator ffvmIt = mp.begin(); ffvmIt != mp.end(); ++ffvmIt) {
        std::cerr << "FILE: " << ffvmIt->first << std::endl;
        for (FunctionVarMap::const_iterator fvmIt = ffvmIt->second.begin(); fvmIt != ffvmIt->second.end(); ++fvmIt) {
            std::cerr << fvmIt->first << std::endl;
            //std::cerr<<handler.sysDict->functionTable.find(fvmIt->first)->second<<std::endl;
            for (VarMap::const_iterator vmIt = fvmIt->second.begin(); vmIt != fvmIt->second.end(); ++vmIt) {
                std::cerr << "-------------------------" << std::endl;
                std::cerr << "Variable: " << vmIt->first << std::endl;
                std::cerr << "Slines: {";
                for (unsigned int sl : vmIt->second.slines) {
                    std::cerr << sl << ",";
                }
                std::cerr << "}" << std::endl;
                std::cerr << "variables dependant on this one: {";
                for (std::string dv : vmIt->second.dvars) {
                    std::cerr << dv << ",";
                }
                std::cerr << "}" << std::endl;
                std::cerr << "is aliase for: {";
                for (std::string al : vmIt->second.aliases) {
                    std::cerr << al << ",";
                }
                std::cerr << "}" << std::endl;
                std::cerr << "cfuntions: {";
                for (auto cfunc : vmIt->second.cfunctions) {
                    std::cerr << cfunc.first << " " << cfunc.second << ",";
                }
                std::cerr << "}" << std::endl;
                std::cerr << "def: {";
                for (auto defv : vmIt->second.def) {
                    std::cerr << defv << ",";
                }
                std::cerr << "}" << std::endl;
                std::cerr << "use: {";
                for (auto usev : vmIt->second.use) {
                    std::cerr << usev << ",";
                }
                std::cerr << "}" << std::endl;
                std::cerr << "-------------------------" << std::endl;
            }
        }
    }
}

std::string join(const char delimiter, std::vector<std::string> source) {
    std::stringstream ss;
    for (size_t i = 0; i < source.size(); ++i) {
        if (i != 0) {
            ss << delimiter;
        }
        ss << source[i];
    }
    return ss.str();
}

/**
 * 関数テーブルから関数を検索して返します
 * @param function_table
 * @param file_path
 * @param func_name
 * @return
 */
FunctionData *find_function(FileFunctionTable* function_table, std::string file_path, std::string func_name) {
    for (auto &func : *function_table) {
        auto func_data = &func.second;
        if (func_data->fileName == file_path
            && func_data->functionName == func_name) {
            return func_data;
        }
    }
    return nullptr;
}

template<typename T>
std::vector<std::string> set_to_vector(std::set<T> lineNumberSet) {
    auto vec = std::vector<T>(std::begin(lineNumberSet), std::end(lineNumberSet));
    std::vector<std::string> out;
    std::transform(
            std::begin(vec),
            std::end(vec),
            std::back_inserter(out), [](T lineNumber) {
                return std::to_string(lineNumber);
            });
    return out;
}

template<typename T1, typename T2>
std::vector<T1> vec_transform(std::vector<T2> vec, std::function<T1(T2)> map) {
    std::vector<std::string> out;
    std::transform(
            std::begin(vec),
            std::end(vec),
            std::back_inserter(out),
            map);
    return out;
}

template<typename T, typename Mapper>
std::vector<std::string> unordered_set_to_vector(std::unordered_set<T> source, Mapper mapper) {
    auto vec = std::vector<T>(std::begin(source), std::end(source));
    std::vector<std::string> out;
    std::transform(
            std::begin(vec),
            std::end(vec),
            std::back_inserter(out), mapper);
    return out;
}

std::string
varmap_pair_to_string(std::string file_name,
                      std::string function_name,
                      std::pair<std::string, SliceProfile> *vmIt,
                      FileFunctionTable* functionTable) {
    std::string str;
    std::string varname = vmIt->first;
    auto sp = vmIt->second;

    std::vector<std::string> container;
    // def{}のうち最小の値を変数のある行番号としている
    unsigned int lineNumber = *sp.def.begin();
    // fileを出力
    container.push_back(file_name + ":" + std::to_string(lineNumber));
    // funcを出力
    container.push_back(function_name);
    // varを出力
    container.push_back(sp.variableType + " " + varname);
    // defを出力
    auto def_line_numbers = set_to_vector<unsigned int>(sp.def);
    // ラムダ式のラムダ導入子([]の部分)に&を入れると、
    // それまでに登場した変数の参照をラムダ式内で利用できる
    // 詳しくはこちら->https://cpprefjp.github.io/lang/cpp11/lambda_expressions.html
    auto mapped = vec_transform<std::string, std::string>(def_line_numbers, [=](std::string lineNumber) {
        auto func = find_function(functionTable, file_name, function_name);
        auto func_id = func->computeId();
        if (func) {
            return lineNumber + "@" + func_id;
        } else {
            return lineNumber + "@<関数ID不明>";
        }
    });

    container.push_back(join(',', mapped));
    // useを出力
    container.push_back(join(',', set_to_vector<unsigned int>(sp.use)));
    // dvarsを出力
    container.push_back(join(',', unordered_set_to_vector<std::string>(sp.dvars, [](std::string x) { return x; })));
    // pointersを出力
    container.push_back(join(',', unordered_set_to_vector<std::string>(sp.aliases, [](std::string x) { return x; })));

    str.append(join('\t', container));

    // cfuncsを出力
    // 余計ややこしくなりそうなので元のコードのままで保留
    str.append("\t");
    for (auto cfunc : sp.cfunctions) {
        std::stringstream ststrm;
        ststrm << cfunc.second;
        str.append(cfunc.first).append("{").append(ststrm.str()).append("},");
    }
    if (str.at(str.length() - 1) == ',')
        str.erase(str.length() - 1);

    return str;
}

/**
 * 変数テーブルを作成します
 * @param dictionary スライスディクショナリ
 * @return 変数テーブルをCSV形式で表した文字列
 */
std::string create_variable_table(SliceDictionary dictionary) {
    std::stringstream ss;

    // ヘッダを出力する
    std::vector<std::string> header({"file", "func", "var", "def", "use", "dvars", "pointers", "cfuncs"});
    ss << join('\t', header) << std::endl;

    // ソートする
    std::map<std::string, FunctionVarMap> sorted_ffvMap
            (dictionary.ffvMap.begin(),
             dictionary.ffvMap.end());
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
            for (std::pair<std::string, SliceProfile> vmIt: sorted_vMap) {
                std::string row = varmap_pair_to_string(ffvmIt.first, fvmIt.first, &vmIt, &dictionary.fileFunctionTable);
                ss << row << std::endl;
            }
        }
    }

    // globalMap も出力する
    auto globalMap = dictionary.globalMap;
    // ソートする
    std::map<std::string, SliceProfile> sorted_globalMap
            (globalMap.begin(), globalMap.end());
    for (std::pair<std::string, SliceProfile> vmIt : sorted_globalMap) {
        std::string row = varmap_pair_to_string(vmIt.second.file, vmIt.second.function, &vmIt,
                                                &dictionary.fileFunctionTable);
        ss << row << std::endl;
    }

    return ss.str();
}

/**
 * 関数テーブルを作成します
 * @param dictionary スライスディクショナリ
 * @return 関数テーブルをCSV形式で表した文字列
 */
std::string create_function_table(SliceDictionary dictionary) {
    std::stringstream ss;

    // ヘッダを出力する
    std::vector<std::string> header({"id", "func_name", "kind", "file_path", "declare_range"});
    ss << join('\t', header) << std::endl;

    // すべての関数について
    for (auto func_pair: dictionary.fileFunctionTable) {
        auto func_data = func_pair.second;

        std::string id = func_data.computeId();
        std::string func_name = func_data.functionName;
        // TODO: 関数の種類を識別できるようにする
        std::string kind = "user-defined";
        std::string file_path = func_data.fileName;
        // FIXME: 関数の定義範囲を取得できるようにする
        std::string declare_range = func_data.declareRange.to_string();

        std::vector<std::string> vec({id, func_name, kind, file_path, declare_range});
        std::string row = join('\t', vec);
        ss << row << std::endl;
    }
    return ss.str();
}

void srcSliceToCsv(const srcSlice &handler) {
    // 変数と関数の解析結果を両方出力するために
    // JSON形式で出力することにした
    // ただし値部分はCSV形式のままなので大した変化はない
    using json = nlohmann::json;

    json j;
    j["vars"] = create_variable_table(handler.dictionary);
    j["funcs"] = create_function_table(handler.dictionary);
    std::cout << j.dump(4) << std::endl;
}

/**
 * main
 * @param argc number of arguments
 * @param argv the provided arguments (array of C strings)
 * 
 * Invoke srcSAX handler to count element occurences and print out the resulting element counts.
 */
/*
  Type Resolution tool
  Def Use Tool as separate thing (same as type res?)
  methods
  statement #
  Consider output to srcML
  */
int main(int argc, char *argv[]) {

    if (argc < 2) {

        std::cerr << "Useage: srcSlice input_file.xml\n";
        exit(1);

    }
    clock_t t;
    t = clock();
    srcSlice sslice;
    sslice.ReadArchiveFile(argv[1]);
    t = clock() - t;
    std::cerr << "Time is: " << ((float) t) / CLOCKS_PER_SEC << std::endl;
    //std::string filename = handler.sysDict->find("stack.cpp.xml");
    //handler.ComputeInterprocedural("SlicerTestSample.cpp");
    //TestSlice(handler.sysDict-> handler);
    //TestSlice2(handler.sysDict->globalMap);
    srcSliceToCsv(sslice);
    return 0;
}