/* my extension for PHP */
#include "stdafx.h"

using namespace std;

namespace my {
    struct TrieNode
    {
        string tag;
        long id;
        long pareId;
        unordered_map<string, TrieNode*>tMap;
        bool isWord;
        int amount; //goods amount
        TrieNode(const string &_tag, long _id, long _pareId, bool _isWord) :tag(_tag), id(_id), pareId(_pareId), isWord(_isWord) {}
    };
    
    class TrieTree
    {
    public:
        TrieNode rootNode;
        boost::object_pool<TrieNode>nodePool;

        TrieTree(TrieNode &_rootNode) :rootNode(_rootNode) {}
        
        ~TrieTree()
        {
            deleteTrieVal(&rootNode);
        }

        void deleteTrieVal(TrieNode *node)
        {
            auto &m = node->tMap;
            for (auto &pa : m) {
                deleteTrieVal(pa.second);
            }
           
            if (node->tag.length() > 0) { //avoid to free rootnode, root node is a stack val
                 nodePool.destroy(node);
            }
        }


        void findTrieStr(const string &inputStr, int pos, TrieNode *pNode, const string &resultStr, string &retStr)
        {
            auto &m = pNode->tMap;

            if (pos < inputStr.length()) {
                int offset = inputStr[pos] < 0 ? 2 : 1;
                string s = inputStr.substr(pos, offset);
               
                auto pIter = m.find(s);
                if (pIter != m.end()) {
                    auto p = pIter->second;
                    string tempS(resultStr);
                    tempS += s;
                    if (p->isWord && pos + offset == inputStr.length()) {
                        retStr += tempS + " ";
                    }
                    findTrieStr(inputStr, pos + offset, p, tempS, retStr);
                }
            }
            else {
                for (auto &pa : m) {
                    auto p = pa.second;
                    string tempS(resultStr);
                    tempS += p->tag;
                    if (p->isWord) {
                        retStr += tempS + " ";
                    }
                    findTrieStr(inputStr, pos, p, tempS, retStr);
                }
            }
        }

        TrieNode*  getNextEquNode(const string &inputStr, TrieNode *pNode, int &pos)
        {
            auto &m = pNode->tMap;
            int offset = inputStr[pos] < 0 ? 2 : 1;
            string s = inputStr.substr(pos, offset);
          
            auto pIter = m.find(s);
            if (pIter != m.end()) {
                pos += offset;
                return getNextEquNode(inputStr, pIter->second, pos);
            }

            return pNode;
        }

        static int getWordsCount(const string &s)
        {
            int pos = 0, count = 0;
            while (pos < s.length()) {
                int offset = s[pos] < 0 ? 2 : 1;
                pos += offset;
                ++count;
            }
            return count;
        }
    };


    class SplitChiClass
    {
    public:
        bool splitEndFlag = false;
        HashTable *dictA = NULL;
        string resultWords;
        enum class HTResult { NOTEXIST, LEGAL, ILLEGAL };
        ~SplitChiClass()
        {
            splitEndFlag = false;
            resultWords.clear();
        }

        short setSplitArray(const string &str, vector<short> &posList)
        {
            short splitPos = 0;
            for (unsigned i = 0; i < str.length(); ) {
                if (str[i] < 0) {
                    i += 2;
                }
                else {
                    do {
                        ++i;
                    } while (str[i] > 0);
                }
                posList.push_back(i);
                ++splitPos;
            }
            return splitPos;
        }

        inline HTResult inHashTable(const string &str)
        {
            //return zend_hash_str_exists(g_dictA, str, strlen(str));
            zval *p = zend_hash_str_find(dictA, str.c_str(), str.length());
            if (!p)
                return HTResult::NOTEXIST;
            else if (Z_TYPE_P(p) == IS_TRUE)
                return HTResult::LEGAL;
            else
                return HTResult::ILLEGAL;
        }

        void splitChiStr(const string &str, vector<short> &posList, short chiNum, int begPos, const string &thisWords)
        {
            int lastPosVal = (begPos == -1 ? 0 : posList[begPos]);

            for (int i = chiNum - 1; i > begPos; --i) {//长词优先
          //for(int i=begPos+1; i<chiNum; ++i){  //短词优先
                if (splitEndFlag)
                    return;
                string wordStr(str, lastPosVal, posList[i] - lastPosVal);

                HTResult existEnum = inHashTable(wordStr);
                if (existEnum != HTResult::NOTEXIST) {
                    if (i == chiNum - 1) { //整句话是1个词说明递归分词完毕
                        splitEndFlag = true;
                        resultWords.clear();
                        resultWords += thisWords;
                        if (existEnum == HTResult::LEGAL) {
                            resultWords += (wordStr + " ");
                        }
                        return;
                    }
                    else {
                        string nextWords(thisWords);
                        if (existEnum == HTResult::LEGAL) {
                            nextWords += (wordStr + " ");
                        }
                        splitChiStr(str, posList, chiNum, i, nextWords);
                    }
                }
            }
            if (resultWords.length() == 0) { //只记录第1次递归到最后并且未成功的结果,如果最终不能分词则返回该结果
                resultWords += (thisWords + str.substr(lastPosVal) + " ");
            }
        }

        static void setRetValue(zval *p, zend_string *str)
        {
            ZVAL_STR(p, str);
        }
    };
}

using namespace my;

static TrieTree g_TrieTree(TrieNode("", 0, -1, false)); //root's id is 0, parentID is -1
PHP_FUNCTION(insertTrie)
{
    int argc = ZEND_NUM_ARGS();
    char * str;
    size_t len;
    if (zend_parse_parameters(argc, "s", &str, &len) == FAILURE)
        return;
    int pos = 0;

    TrieNode *pNode = g_TrieTree.getNextEquNode(str, &g_TrieTree.rootNode, pos);

    string inputStr(str);
    TrieNode *pNewNode = NULL;
    while (pos < inputStr.length()) {
        auto &m = pNode->tMap;
        int offset = inputStr[pos] < 0 ? 2 : 1;
        string tag = inputStr.substr(pos, offset);

        pNewNode=g_TrieTree.nodePool.construct(TrieNode(tag, -2, pNode->id, false)); //TODO -2 is invalid
        m[tag] = pNewNode;
        pNode = pNewNode;
        pos += offset;

        int level = TrieTree::getWordsCount(inputStr);
        printf("tag:%s, pareId:%d\n", tag.c_str(), pNode->id); //TODO insert database (tag, lepareId)
      //pNode->id =  get database auto insert id; 
    }
    pNewNode->isWord = true;
}

PHP_FUNCTION(searchTrie)
{
    int argc = ZEND_NUM_ARGS();
    char * str;
    size_t len;
    if (zend_parse_parameters(argc, "s", &str, &len) == FAILURE)
        return;
    string retStr;
    g_TrieTree.findTrieStr(str, 0, &g_TrieTree.rootNode, "", retStr);
    RETURN_STRING(retStr.c_str());
}


/* {{{ PHP_RINIT_FUNCTION
*/
PHP_RINIT_FUNCTION(my)
{
#if defined(ZTS) && defined(COMPILE_DL_MY)
    ZEND_TSRMLS_CACHE_UPDATE();
#endif

    return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
*/
PHP_MINFO_FUNCTION(my)
{
    php_info_print_table_start();
    php_info_print_table_header(2, "my support", "enabled");
    php_info_print_table_end();
}
/* }}} */



/* {{{ my_functions[]
*/
static const zend_function_entry my_functions[] = {
    PHP_FE(searchTrie, NULL)
    PHP_FE(insertTrie, NULL)
    PHP_FE_END
};
/* }}} */

/* {{{ my_module_entry
*/
zend_module_entry my_module_entry = {
    STANDARD_MODULE_HEADER,
    "my",					/* Extension name */
    my_functions,			/* zend_function_entry */
    NULL,							/* PHP_MINIT - Module initialization */
    NULL,							/* PHP_MSHUTDOWN - Module shutdown */
    PHP_RINIT(my),			/* PHP_RINIT - Request initialization */
    NULL,							/* PHP_RSHUTDOWN - Request shutdown */
    PHP_MINFO(my),			/* PHP_MINFO - Module info */
    PHP_MY_VERSION,		/* Version */
    STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_MY
# ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
# endif
ZEND_GET_MODULE(my)
#endif

