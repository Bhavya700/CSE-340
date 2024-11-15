#ifndef __PARSER__H__
#define __PARSER__H__

#include <stack>
#include "lexer.h"

struct varDetail {
    std::string scope;
    std::string name;
    int type;
};

struct var_list {
    varDetail item;
    var_list* prev;
    var_list* next;
};

class Parser {
    public:
        void programparser();
        struct var_list *table;

    private:
        LexicalAnalyzer lexer;
        std::string cScope;
        std::stack<std::string> Scopes;
        std::string output(std::string);
        void syntax_error();
        void scopeparser();
        void variable(int);
        void declar_var();
        void global_var();
        void public_var();
        void private_var();
        void assgin_var();
};

#endif