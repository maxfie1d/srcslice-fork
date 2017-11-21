#pragma once

#include <climits>

/**
 * ソースファイルにおける範囲を行番号で表す
 * 行番号は1行目を1と表す(one-based)
 */
struct ProgramRange {

    static const unsigned int DEFAULT_VALUE = UINT_MAX;

    unsigned int startLine;
    unsigned int endLine;

    ProgramRange(
            unsigned int startLine = DEFAULT_VALUE,
            unsigned int endLine = DEFAULT_VALUE
    ) {
        this->startLine = startLine;
        this->endLine = startLine;
    }

    /**
     * 開始行はセットされているか
     * @return
     */
    bool isStartLineSet() {
        return this->startLine != DEFAULT_VALUE;
    }

    /**
     * 終了行はセットされているか
     * @return
     */
    bool isEndLineSet() {
        return this->endLine != DEFAULT_VALUE;
    }

    /**
     * 開始行と終了行を初期化する
     */
    void clear() {
        this->startLine = this->endLine = DEFAULT_VALUE;
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

    NameAndLineNumber(std::string name = std::string(""),
                      unsigned int lineNumber = 0) {
        this->name = name;
        this->lineNumber = lineNumber;
    }
};
