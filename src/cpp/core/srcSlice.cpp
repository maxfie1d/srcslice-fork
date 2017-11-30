#include <core/srcSlice.hpp>
#include "helpers/FileHelper.h"
#include "helpers/srcMLHelper.h"

srcSlice::srcSlice(const char *filename, const char *encoding = 0) {
    srcSAXController control(filename);
    srcSliceHandler handler(&dictionary);
    control.parse(&handler);
    handler.computeAllInterproceduralRelation();
}

srcSlice::srcSlice(std::string buffer, const char *encoding = 0) {
    srcSAXController control(reconstructSrcMLStringForSrcSlice(buffer));
    srcSliceHandler handler(&dictionary);
    control.parse(&handler);
    handler.computeAllInterproceduralRelation();
}

srcSlice::srcSlice(FILE *file, const char *encoding = 0) {
    srcSAXController control(file);
    srcSliceHandler handler(&dictionary);
    control.parse(&handler);
    handler.computeAllInterproceduralRelation();
}

srcSlice::srcSlice(int fd, const char *encoding = 0) {
    srcSAXController control(fd);
    srcSliceHandler handler(&dictionary);
    control.parse(&handler);
    handler.computeAllInterproceduralRelation();
}

void srcSlice::ReadArchiveFile(std::string filename) {
    //can call if default contructed

    // srcml文書から
    std::string srcmlStr = reconstructSrcMLStringForSrcSlice(readFileAsStr(filename.c_str()));

    // ファイルごとのスライスを計算し
    srcSAXController control(srcmlStr);
    srcSliceHandler handler(&dictionary);
    control.parse(&handler);

    // 手続き(ファイル)間の関係を計算する
    handler.computeAllInterproceduralRelation();
}