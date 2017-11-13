#pragma once

#include <climits>

/**
 * ソースファイルにおける範囲を行番号で表す
 * 行番号は1行目を1と表す(one-based)
 */
struct ProgramRange {
    unsigned int startLine;
    unsigned int endLine;

    ProgramRange(
            unsigned int startLine = UINT_MAX,
            unsigned int endLine = UINT_MAX
    ) {
        this->startLine = startLine;
        this->endLine = startLine;
    }

    std::string to_string() {
        return std::to_string(this->startLine) + "-" + std::to_string(this->endLine);
    }
};

/**
 * 名前と行番号のペア
 */
struct NameAndLineNumber {
    std::string name;
    unsigned int lineNumber;

    // 形名に()をつけると、デフォルト値が入ることになるぞ！

    NameAndLineNumber(std::string name = std::string(),
                      unsigned int lineNumber = 0) {
        this->name = name;
        this->lineNumber = lineNumber;
    }
};
