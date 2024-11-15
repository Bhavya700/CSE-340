#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>
#include <stdlib.h>
#include "lexer.h"
#include "inputbuf.h"
#include "parser.h"
#include <stack>

using namespace std;

string FOut;

void Parser::programparser() {
    cScope = "::";
    Scopes.push(cScope);
    Token t = lexer.GetToken();
    if (t.token_type == ID) {
        Token t2 = lexer.GetToken();
        if (t2.token_type == SEMICOLON || t2.token_type == COMMA) {
            lexer.UngetToken(t2);
            lexer.UngetToken(t);
            global_var();
            scopeparser();
        } 
        else if (t2.token_type == LBRACE) {
            lexer.UngetToken(t2);
            lexer.UngetToken(t);
            scopeparser();
        } 
        else {
            syntax_error();
        }
    } 
    else {
        syntax_error();
    }
}

void Parser::syntax_error() {
    cout<<"Syntax Error\n";
    FOut.clear();
    exit(1);
}

void Parser::scopeparser() {
    Token t = lexer.GetToken();
    Token t2 = lexer.GetToken();
    if (t.token_type == ID && t2.token_type == LBRACE) {
        cScope = t.lexeme;
        Scopes.push(cScope);
        public_var();
        private_var();
        assgin_var();
        cScope = Scopes.top();
        Scopes.pop();
        Token t3 = lexer.GetToken();
        if (t3.token_type != RBRACE) {
            syntax_error();
        }
    } 
    else {
        syntax_error();
    }
}

void Parser::global_var() {
    Token t = lexer.GetToken();
    if (t.token_type == ID) {
        lexer.UngetToken(t);
        variable(1);
    } 
    else {
        lexer.UngetToken(t);
        return;
    }
    t=lexer.GetToken();
    if (t.token_type != SEMICOLON) {
        syntax_error();
    }
}

void Parser::public_var() {
    Token t = lexer.GetToken();
    if (t.token_type == PUBLIC) {
        Token t2 = lexer.GetToken();
        if (t2.token_type == COLON) {
            variable(1);
            Token t3 = lexer.GetToken();
            if (t3.token_type != SEMICOLON) {
                syntax_error(); 
            } 
        } 
        else {
            syntax_error();
        }
    } 
    else {
        lexer.UngetToken(t);
    }
}

void Parser::private_var(){
    Token t = lexer.GetToken();
    if (t.token_type == PRIVATE) {
        Token t2 = lexer.GetToken();
        cout << t2.lexeme;
        if (t2.token_type == COLON) {
            variable(0);
            Token t3 = lexer.GetToken();
            if (t3.token_type != SEMICOLON) {
                syntax_error();
            } 
        } 
        else {
            syntax_error();
        }
    } 
    else {
        lexer.UngetToken(t);
    }
}

void Parser::variable(int type) {
    Token t = lexer.GetToken();
    if (t.token_type == ID) {
        Token t2 = lexer.GetToken();
        if (t2.token_type == COMMA) {
            variable(type);
        } 
        else {
            lexer.UngetToken(t2);
        }
        struct varDetail newItem;
        newItem.scope = cScope;
        newItem.type = type;
        newItem.name = t.lexeme;

        struct var_list *newTable = new struct var_list;
        newTable->item = newItem;
        newTable->next = table;
        table = newTable;
    } 
    else {
        syntax_error(); 
    }
}

void Parser::assgin_var() {
    Token t = lexer.GetToken();
    if (t.token_type == ID) {
        Token t2 = lexer.GetToken();
        if (t2.token_type == EQUAL) {
            lexer.UngetToken(t2);
            lexer.UngetToken(t);
            declar_var();
        } 
        else if (t2.token_type == LBRACE) {
            lexer.UngetToken(t2);
            lexer.UngetToken(t);
            scopeparser();
        } 
        else {
            syntax_error();
        }
        Token t3 = lexer.GetToken();
        if (t3.token_type == ID) {
            lexer.UngetToken(t3);
            assgin_var();
        } 
        else {
            lexer.UngetToken(t3);
        }

    } 
    else {
        syntax_error();
    }
}

void Parser::declar_var() {
    Token t = lexer.GetToken();
    if (t.token_type == ID) {
        Token t2 = lexer.GetToken();
        if (t2.token_type == EQUAL) {
            Token t3 = lexer.GetToken();
            Token t4 = lexer.GetToken();
            if ( t3.token_type == ID && t4.token_type == SEMICOLON) {
                FOut+=output(t.lexeme);
                FOut+=t.lexeme;
                FOut+=" = "; 
                FOut+=output(t3.lexeme);
                FOut+=t3.lexeme;
                FOut+='\n';
            } 
            else {
                syntax_error();
            }
        } 
        else {
            lexer.UngetToken(t2);
            lexer.UngetToken(t);
            scopeparser();
        }
    } 
    else {
        syntax_error();
    }
}

string Parser::output(string id) {
    struct var_list cur_list = *table;
    bool found = false;
    while (cur_list.next != NULL && !cur_list.item.name.empty() && !found) {
        if (cur_list.item.name == id && (cur_list.item.scope == cScope || cur_list.item.type)) {
            found = true;
        } 
        else {
            cur_list = *cur_list.next; 
        }
    }
    if (!cur_list.item.name.empty() && cur_list.item.scope == "::") {
        return "::";
    } 
    else if (!cur_list.item.name.empty() && cur_list.item.name == id ) {
        return cur_list.item.scope + ".";
    } 
    else {
        return "?.";
    }
}

int main(){
    Parser P;
    P.table = new struct var_list();
    P.programparser();
    cout<<FOut;
    return 0;
}
