#pragma once

#include <iostream>
#include <fstream>

/**
 * ファイルをすべて読みこんで文字列として返します
 * @param filename
 * @return
 */
std::string readFileAsStr(const char *filename);

/**
 * プロジェクトルートを基準とした相対パスを絶対パスに解決します
 * @param path
 * @return
 */
std::string resolvePath(std::string path);
