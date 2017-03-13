#include "skip_list.h"
#include <stdio.h>
#include <stdlib.h>
#include <sstream>

namespace my_code {

static std::string itoa(int i) {
    std::stringstream ss;
    ss << i;
    return ss.str();
}

SkipList::SkipList(): _level(0) {
}

SkipList::~SkipList() {

}

int SkipList::init() {
    return init(DEFAULT_MAX_LEVEL);
}

int SkipList::init(int max_level) {
    if (max_level <= 0) {
        fprintf(stderr, "max_level: %d error", max_level);
        return -1;
    }

    // TODO set maximum value of max_level?
    _max_level = max_level;

    _head = create_node(_max_level, -1, -1);
    if (_head == NULL) {
        fprintf(stderr, "create node fail, max_level: %d", _max_level);
        return -1;
    }

    for (int i = 0; i < _max_level; ++i) {
        _head->forward[i] = NULL;
    }

    return 0;
}

int SkipList::destroy() {
    Node* cur = _head;
    Node* next;
    while (cur != NULL) {
        next = cur->forward[0];
        free(cur);
        cur = next;
    }

    return 0;
}

int SkipList::insert(KeyType key, ValueType value) {
    int level = random_level();
    Node* node = create_node(level, key, value);
    if (node == NULL) {
       fprintf(stderr, "create_node fail, level: %d", level);
       return -1;
    }

    Node* prefix[_max_level];
    // start from _level, because level may be smaller than _level such as level=1, then
    // from _level will be fast to find the appropriate location.
    for (int i = _level; i >= 0; --i) {
        Node* cur = _head->forward[i];
        Node* p = _head;
        while (cur != NULL && cur->key < key) {
            p = cur;
            cur = cur->forward[i];
        }
        if (cur != NULL && cur->key == key) {
            free(node);
            printf("key: %d already exist", key);
            return 1;
        }
        prefix[i] = p;
    }
    // level-1 may be larger than _level
    for (int i = _max_level-1; i > _level; i--) {
        prefix[i] = _head;
    }

    for (int i = level - 1; i >= 0; --i) {
        node->forward[i] = prefix[i]->forward[i];
        prefix[i]->forward[i] = node;
    }
    
    if (level - 1 > _level) {
        _level = level - 1;
    }

    return 0;
}

int SkipList::remove(KeyType key) {
    Node* prefix[_level+1];

    Node* cur = NULL;
    int key_level = -1;
    for (int i = _level; i >= 0; --i) {
        cur = _head->forward[i];
        Node* p = _head;
        while (cur != NULL && cur->key < key) {
            p = cur;
            cur = cur->forward[i];
        }
        
        if (cur != NULL && cur->key == key) {
            key_level = (key_level > i) ? key_level : i;
        }

        prefix[i] = p;
    }
    if (key_level == -1 || cur == NULL || (cur != NULL && cur->key != key)) {
        printf("key: %d not found", key);
        return 1;
    }

    printf("key_level: %d\n", key_level);
    // currently cur point to the key node
    for (int i = key_level; i >= 0; --i) {
        prefix[i]->forward[i] = cur->forward[i];
    }
    
    for (int i = _level; i >= 0; --i) {
        if (_head->forward[i] == NULL) {
            _level--;
        } else {
            break;
        }
    }

    free(cur);
    return 0;
}

// O(logN) time
ValueType* SkipList::search(KeyType key) {
    Node* cur = NULL;
    Node* prefix = _head;
    for (int i = _level; i >= 0; --i) {
        cur = prefix->forward[i];
        while (cur != NULL && cur->key < key) {
            prefix = cur;
            cur = cur->forward[i];
        }
        
        if (cur != NULL && cur->key == key) {
            return &(cur->value);
        } 
    }

    return NULL;
}

bool SkipList::contains(KeyType key) {
    ValueType* v_p = search(key);

    return (v_p != NULL) ? true : false;
}

Node* SkipList::find_last() {
    Node* cur = NULL;
    Node* prefix = _head;
    for (int i = _level; i >= 0; --i) {
        cur = prefix->forward[i];
        while(cur != NULL) {
            prefix = cur;
            cur = cur->forward[i];
        }
    }

    if (prefix == _head) {
        return NULL;
    }

    return prefix;
}

// O(logN) time
Node* SkipList::find_less_than(KeyType key) {
    Node* cur = NULL;
    Node* prefix = _head;
    for (int i = _level; i >= 0; --i) {
        cur = prefix->forward[i];
        while(cur != NULL && cur->key < key) {
            prefix = cur;
            cur = cur->forward[i];
        }
    }
    if (prefix == _head) {
        return NULL;
    }

    return prefix;
}

// O(logN) time
Node* SkipList::find_greater_or_equal(KeyType key) {
    Node* cur = NULL;
    Node* prefix = _head;
    for (int i = _level; i >= 0; --i) {
        cur = prefix->forward[i];
        while (cur != NULL && cur->key < key) {
            prefix = cur;
            cur = cur->forward[i];
        }
        if (cur != NULL && cur->key == key) {
            return cur;
        }
    }

    return cur;
}

std::string SkipList::to_str() {
    std::string str;
    
    for (int i = _level; i >= 0; --i) {
        str += "head->";
        Node* cur = _head->forward[i];
        while (cur != NULL) {
            str += itoa(cur->key) + std::string("->");
            cur = cur->forward[i];
        }
        str += "NULL\n";
    }
    
    return str;
}

/*
 * 1: 100%
 * 2: 50%
 * 3: 25%
 * 4: 12.5%
 * ...
 */
int SkipList::random_level() {
    int level = 1;

    while (rand() % 2 == 0) {
        level++;
    }
    level = (level > _max_level) ? _max_level: level;

    return level;
}

// alloc a Node which has #level of Node* pointers.
Node* SkipList::create_node(int level, KeyType key, ValueType value) {
    if (level <= 0) {
        fprintf(stderr, "not legal level: %d", level);
        return NULL;
    }

    // here choose level-1 because there already exists a Node* pointer in Node.
    Node* node = (Node*)malloc(sizeof(Node) + sizeof(Node*) * (level-1));
    if (node == NULL) {
        fprintf(stderr, "malloc fail, level: %d", level);
        return NULL;
    }
    node->key = key;
    node->value = value;

    return node;
}

}
