#pragma once


#include <libxml/parser.h>
#include <vector>
#include <iostream>

/**
 * XMLノードを走査して、要素名を出力する関数
 * @param node
 * @param depth
 */
void dump_xml_elements(xmlNodePtr node, int depth);

/**
 * ヘッダファイルが上位に来るように、srcML文書を
 * 再構成します
 * @param srcmlStr
 * @return
 */
std::string reconstructSrcMLStringForSrcSlice(std::string srcmlStr);
