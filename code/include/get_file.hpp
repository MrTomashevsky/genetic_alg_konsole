#ifndef GET_FILE_HPP
#define GET_FILE_HPP

#include "fstream"
#include "string"

static std::string get_file(std::string fn, bool *flag = nullptr) {
    std::string str;
    std::ifstream fin(fn);
    if (flag)
        *flag = false;
    if (fin.is_open()) {
        if (flag)
            *flag = true;

        std::copy(std::istreambuf_iterator<char>(fin), std::istreambuf_iterator<char>(), std::back_inserter(str));
        for (auto it = str.begin(); it < str.end(); it++) {
            if (*it == '\t') {
                *it = ' ';
                it  = str.insert(it, 3, ' ');
            }
        }
        fin.close();
    }
    return str;
};

#endif  // GET_FILE_HPP
