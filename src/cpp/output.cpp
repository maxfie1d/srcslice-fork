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
#include "functional.hpp"


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
                      SliceProfile *vmIt) {
    std::string str;
    std::string varname = vmIt->variableName;
    auto sp = vmIt;

    std::vector<std::string> container;

    // idを出力
    container.push_back(sp->computeVariableId());
    // def{}のうち最小の値を変数のある行番号としている
    unsigned int lineNumber = sp->def.begin()->lineNumber;
    // fileを出力
    container.push_back(file_name + ":" + std::to_string(lineNumber));
    // funcを出力
    container.push_back(function_name);
    // varを出力
    container.push_back(sp->variableType + " " + varname);
    // defを出力
    auto def_line_numbers = set_to_vector<ProgramPoint>(sp->def);
    auto defs_as_string = vec_transform<ProgramPoint, std::string>(def_line_numbers, [](ProgramPoint pp) {
        return pp.to_string();
    });
    container.push_back(join(',', defs_as_string));

    // useを出力
    auto use_line_numbers = set_to_vector<ProgramPoint>(sp->use);
    auto uses_as_string = vec_transform<ProgramPoint, std::string>(use_line_numbers, [](ProgramPoint pp) {
        return pp.to_string();
    });
    container.push_back(join(',', uses_as_string));

    // dvarsを出力
    auto dvars = set_to_vector<DvarData>(sp->dvars);
    auto dvars_as_string = vec_transform<DvarData, std::string>(dvars, [](DvarData dd) {
        return dd.to_string();
    });
    container.push_back(join(',', dvars_as_string));

    // pointersを出力
    container.push_back(join(',', unordered_set_to_vector<std::string>(sp->aliases, [](std::string x) { return x; })));

    // cfuncsを出力
    auto cfuncs = vec_transform<CFuncData, std::string>(set_to_vector<CFuncData>(sp->cfunctions), [](CFuncData cfd) {
        return cfd.to_string();
    });
    container.push_back(join(',', cfuncs));

    // タブ区切りでつなげる
    str.append(join('\t', container));
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
    std::vector<std::string> header({"id", "file", "func", "var", "def", "use", "dvars", "pointers", "cfuncs"});
    ss << join('\t', header) << std::endl;

    dictionary.ffvMap.forEach([](SliceProfile *sp) {
        std::string row = varmap_pair_to_string(sp->file, sp->function, sp);
        ss << row << std::endl;
    });

    // globalMap も出力する
    auto globalMap = dictionary.globalMap;
    // ソートする
    std::map<std::string, SliceProfile> sorted_globalMap
            (globalMap.begin(), globalMap.end());
    for (auto vmIt : sorted_globalMap) {
        std::string row = varmap_pair_to_string(vmIt.second.file, vmIt.second.function, &vmIt.second);
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
    for (auto func_pair: dictionary.functionTable) {
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