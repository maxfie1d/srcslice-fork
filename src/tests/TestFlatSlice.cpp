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
#include <srcSAXController.hpp>
#include <time.h>
#include <map>
#include <iostream>
#include <srcml.h>
#include <cassert>
#include <fstream>
#include "TestFlatSlice.hpp"
#include "TestHelper.h"

/// <summary>
/// Utility function that trims from the right of a string. For now it's just solving a weird issue with srcML
/// and garbage text ending up at the end of the cstring it returns.
/// </summary>
inline char *TrimFromEnd(char *s, size_t len) {
    for (int i = len - 1; i > 0; --i) {
        if (s[i] != '>') {
            s[i] = 0;
        } else {
            return s;
        }
    }
    return nullptr;
}


void testApp1() {
    const char *path = "../../src/tests/samples/app1.c";
    std::string content = readFileAsStr(path);
    std::cout << content << std::endl;
}

int main(int argc, char **argv) {
    // Extra content at the end of the document になってしまう。
    // 確かにsrcmL形式に変換された文字列を見てみると変な文字が末尾にある。
    // srcmlのバイナリで変換したものには見られないので、srcslice内の処理が
    // 何かが起きていると思われる。

    std::cout << "TestApp1 を実行します" << std::endl;
    testApp1();

    //srcTypeNS::srcType typeDict;
    //typeDict.ReadArchiveFile(argv[1]);
    //typeDict.SerializeMap(SerializeToCppUMap);
    //std::cerr<<typeDict.size();
}
