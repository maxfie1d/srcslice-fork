#include <helpers/StringHelper.h>
#include <sstream>
#include <vector>
#include <picosha2.h>

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

std::string computeSHA256Hash(std::string source) {
    // SHA256ハッシュを求める
    // 4バイト用意すると、16進数文字列としたときに
    // 8文字になる
    std::vector<unsigned char> hash(4);
    picosha2::hash256(std::begin(source), std::end(source),
                      std::begin(hash), std::end(hash));

    std::string hex_hash = picosha2::bytes_to_hex_string(std::begin(hash), std::end(hash));
    return hex_hash;
}
