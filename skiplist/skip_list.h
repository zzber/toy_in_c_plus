#ifndef MY_SKIP_LIST_H
#define MY_SKIP_LIST_H

#include <string>

typedef int KeyType;
typedef int ValueType;

#define DEFAULT_MAX_LEVEL 16
#define DEBUG_MODE 1

namespace my_code {

struct Node {
    KeyType key;
    ValueType value;

    Node* forward[1];
};

class SkipList {
public:
    SkipList();

    ~SkipList();
    
    int init();

    /*
     * @return value:
     * 0: init succ
     * -1: init fail
     */
    int init(int max_level);

    int destroy();

    /*
     * @return value:
     * 0: insert succ
     * 1: key exist already
     * -1: insert fail
     */
    int insert(KeyType key, ValueType value);
    
    /*
     * @return value:
     * 0: remove succ
     * 1: key not exist
     */
    int remove(KeyType key);

    ValueType* search(KeyType key);

    std::string to_str();
private:
    int random_level();

    Node* create_node(int level, KeyType key, ValueType value);
private:
    int _max_level; // max_level of skip_list
    int _level; // current level, start from 0..._max_level
    Node* _head;
};

} // namespace my_code

#endif  //__SKIP_LIST_H_
