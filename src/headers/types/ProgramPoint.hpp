#pragma once

#include <string>

/**
 * 行番号と関数IDによって
 * どの関数の中かという情報、つまりプログラム中での場所を
 * 表します。行番号は同じ関数内での前後関係を使う、つもりです。
 */
struct ProgramPoint {
    unsigned int lineNumber;
    std::string functionId;

    ProgramPoint() = default;

    ProgramPoint(unsigned int lineNumber, std::string functionId) {
        this->lineNumber = lineNumber;
        this->functionId = functionId;
    }

    bool operator<(const ProgramPoint &other) const {
        if (this->lineNumber == other.lineNumber) {
            return this->functionId < other.functionId;
        } else {
            return this->lineNumber < other.lineNumber;
        }
    }

    std::string to_string() {
        return std::to_string(this->lineNumber) + "@" + this->functionId;
    }
};
