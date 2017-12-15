#include <core/SliceProfile.hpp>
#include <helpers/StringHelper.h>
#include <sstream>

void FunctionData::clear() {
    this->returnType.clear();
    this->functionName.clear();
    this->declareRange.clear();
}

/// 関数IDの計算方法
/// <関数名>@<その関数があるソースファイルパス>のSHA256ハッシュの先頭8文字
/// \return
std::string FunctionData::computeId() {
    if (this->id.empty()) {
        if (this->functionName.empty() || this->fileName.empty()) {
            throw "関数名かファイルパスが不明のため関数IDを計算できません";
        } else {
            std::string concat = this->functionName + "@" + this->fileName;
            std::string hex_hash = computeSHA256Hash(concat);
            this->id = hex_hash;
            return hex_hash;
        }
    } else {
        return this->id;
    }
}

std::string SliceProfile::computeVariableId() {
    if (this->id.empty()) {
        if (this->file.empty() || this->function.empty() || this->variableName.empty()) {
            return "<unknown>";
        } else {
            std::string concat = this->variableName + "@" + this->function + "@" + this->file.file_path;
            std::string hex_hash = computeSHA256Hash(concat);
            this->id = hex_hash;
            return hex_hash;
        }
    } else {
        return this->id;
    }
}

bool CFuncData::operator<(const CFuncData &other) const {
    return this->location < other.location;
}

std::string CFuncData::to_string() const {
    std::stringstream ss;
    ss << this->calledFunctionName << "(" << this->calledFunctionId << "){" << this->argIndenx.to_string() << "}:"
       << this->location.to_string();
    return ss.str();
}
