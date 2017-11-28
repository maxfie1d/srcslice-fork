#include "srcMLHelper.h"

bool strEndsWith(const std::string &str, const std::string &suffix) {
    return str.size() >= suffix.size() &&
           str.compare(str.size() - suffix.size(), suffix.size(), suffix) == 0;
}

/**
 * XMLノードのfilename属性を読み取る
 * @param node
 * @return
 */
std::string getFileNameFromXmlNode(xmlNodePtr node) {
    for (auto it = node->properties; it != nullptr; it = it->next) {
        if (!xmlStrcmp(it->name, (const unsigned char *) "filename")) {
            auto value = xmlNodeListGetString(node->doc, it->children, 1);
            auto s = std::string((const char *) value);
            xmlFree(value);
            return s;
        }
    }
    return nullptr;
}

std::vector<_xmlNode *> extractFilesWithExtension(_xmlNode *root_node, const char *extension) {
    std::vector<_xmlNode *> nodes;
    for (auto it = root_node->children; it != nullptr; it = it->next) {
        if (it->type != XML_TEXT_NODE) {
            if (!xmlStrcmp(it->name, (const unsigned char *) "unit")) {
                std::string filename = getFileNameFromXmlNode(it);
                // 拡張子をチェック
                if (strEndsWith(filename, extension)) {
                    nodes.push_back(it);
                }
            }
        }
    }
    return nodes;
}

/**
 * XMLノードをつなぎ直します(->prev, ->next)
 * @param nodes
 */
void reconnectXmlNodes(std::vector<_xmlNode *> *nodes) {
    auto n = *nodes;
    for (int i = 0; i < n.size(); ++i) {
        auto h = n[i];
        if (i == 0) {
            h->prev = nullptr;
            h->next = n[i + 1];
        } else if (i == nodes->size() - 1) {
            h->prev = n[i - 1];
            h->next = nullptr;
        } else {
            h->prev = n[i - 1];
            h->next = n[i + 1];
        }
    }
}

/**
 * Unit数(=ファイル数)をカウントします
 * @param root_node
 * @return
 */
unsigned int countUnits(xmlNodePtr root_node) {
    unsigned int count = 0;
    for (auto it = root_node->children; it != nullptr; it = it->next) {
        if (!xmlStrcmp(it->name, (const unsigned char *) "unit")) {
            ++count;
        }
    }
    return count;
}

/**
 * ヘッダファイルが先に来るように
 * XMLを作り直す
 * @param original_root_node
 */
std::string reconstruct(xmlNodePtr original_root_node) {
    xmlDocPtr doc = xmlNewDoc((const unsigned char *) "1.0");
    auto new_root_node = xmlNewNode(NULL, (const unsigned char *) "unit");
    xmlDocSetRootElement(doc, new_root_node);

    // ファイル名の拡張子を見てUnitを分類する
    std::vector<_xmlNode *> headers = extractFilesWithExtension(original_root_node, ".h");
    std::vector<_xmlNode *> cpps = extractFilesWithExtension(original_root_node, ".c");

    // ポインタをつなぎ変える
    reconnectXmlNodes(&headers);
    reconnectXmlNodes(&cpps);

    // headersの末尾とcppの先頭をつなぐ
    headers[headers.size() - 1]->next = cpps[0];
    cpps[0]->prev = headers[headers.size() - 1];

    // xmlAddChildという関数もあるが、
    // よくわからない挙動をするので使わなかった
    new_root_node->children = headers[0];

    xmlChar *s;
    int size;
    xmlDocDumpMemory(doc, &s, &size);

    // ここでsを出力する
    auto result = std::string((const char *) s);
    xmlFree(s);

    return result;
}

std::string reconstructSrcMLStringForSrcSlice(std::string srcmlStr) {
    xmlDocPtr doc = xmlParseDoc((unsigned char *) srcmlStr.c_str());
    xmlNodePtr original_root_node = xmlDocGetRootElement(doc);

    // Unit数が1、つまり単一ソースファイルの時は
    // XMLを再構成する必要がない
    unsigned int unit_count = countUnits(original_root_node);
    std::string result = unit_count > 1
                         ? reconstruct(original_root_node)
                         : srcmlStr;

    xmlFreeDoc(doc);
    xmlCleanupParser();

    return result;
}

void dump_xml_elements(xmlNodePtr node, int depth) {
    for (auto it = node; it != nullptr; it = it->next) {
        if (it->type != XML_TEXT_NODE) {
            for (int i = 0; i < depth; ++i) {
                std::cout << " ";
            }
            std::cout << it->name << std::endl;
        }
        dump_xml_elements(it->children, depth + 1);
    }
}

