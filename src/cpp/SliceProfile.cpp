#include <SliceProfile.hpp>
#include <picosha2.h>


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
            // SHA256ハッシュを求める

            // 4バイト用意すると、16進数文字列としたときに
            // 8文字になる
            std::vector<unsigned char> hash(4);
            picosha2::hash256(std::begin(concat), std::end(concat),
                              std::begin(hash), std::end(hash));

            std::string hex_hash = picosha2::bytes_to_hex_string(std::begin(hash), std::end(hash));

            this->id = hex_hash;
            return hex_hash;
        }
    } else {
        return this->id;
    }
}
