#include <iostream>
#include <string>
#include "skip_list.h"

using namespace std;
using namespace my_code;

int main(int argc, char* argv[]) {
    srand(time(0));
    int key = atoi(argv[1]);

    SkipList sl;

    int ret = sl.init();
    if (ret != 0) {
        cout << "skip list init fail" << endl;
        return 1;
    }

    for (int i = 0; i < 100; ++i) {
        ret = sl.insert(i, i*i);
    }
    string str = sl.to_str();
    cout << str << endl;
    
    int* p_val = sl.search(key);
    if (p_val != NULL) {
        cout << key << " found, val: " << *p_val << endl;
    } else {
        cout << key << " not found" << endl;
    }

    ret = sl.remove(key);
    if (!ret) {
        cout << key << " removed succ" << endl;
    } else {
        cout << key << " removed fail" << endl;
    }

    str = sl.to_str();
    cout << str << endl;

    return 0;
}
