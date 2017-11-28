#include <srcSlice.hpp>
#include "FileHelper.h"
#include "srcMLHelper.h"

srcSlice::srcSlice(const char *filename, const char *encoding = 0) {
    srcSAXController control(filename);
    srcSliceHandler handler(&dictionary);
    control.parse(&handler);
    DoComputation(handler, handler.sysDict->variableTable);
}

srcSlice::srcSlice(std::string buffer, const char *encoding = 0) {
    srcSAXController control(reconstructSrcMLStringForSrcSlice(buffer));
    srcSliceHandler handler(&dictionary);
    control.parse(&handler);
    DoComputation(handler, handler.sysDict->variableTable);
}

srcSlice::srcSlice(FILE *file, const char *encoding = 0) {
    srcSAXController control(file);
    srcSliceHandler handler(&dictionary);
    control.parse(&handler);
    DoComputation(handler, handler.sysDict->variableTable);
}

srcSlice::srcSlice(int fd, const char *encoding = 0) {
    srcSAXController control(fd);
    srcSliceHandler handler(&dictionary);
    control.parse(&handler);
    DoComputation(handler, handler.sysDict->variableTable);
}

void srcSlice::ReadArchiveFile(std::string filename) {
    //can call if default contructed
    std::string srcmlStr = reconstructSrcMLStringForSrcSlice(readFileAsStr(filename.c_str()));
    srcSAXController control(srcmlStr);
    srcSliceHandler handler(&dictionary);
    control.parse(&handler);
    DoComputation(handler, handler.sysDict->variableTable);
}