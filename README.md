# autocomplete
A C++ library that convert the data from database to a trie tree which is constructed in memory. The C++ classes are wrapped in a php extension. It supports the multibyte character like Chinese. You can use this library in 2 occasions below:
1)	You can build a php central server by providing English/Chinese words autocomplete json/socket services to php-fpm servers. You need load all the data from database before you use pcntl_fork to build php process pool. Attention, you cannot update the trie tree except you restart the process pool.

2)	You can embed this library into a thread pool written by C++ which provides autocomplete services. Then you can update the trie tree at any time if you add some mutex locks before your modification.

The source code is a visual studio dll project that you can build a php extension based on PHP7.3. After you load the extension into php environment, you can test its function by using some php codes like this:
/////////////////////////////////
insertTrie("本田");
insertTrie("本来");
insertTrie("本田车");
insertTrie("丰田");
insertTrie("丰田车");
insertTrie("中国");
echo "autocomplete results:" . searchTrie("本") . "\n";
///////////////////////////////
The results are: 本田 本来 本田车
