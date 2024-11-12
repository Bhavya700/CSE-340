/*
 * Copyright (C) Rida Bazzi, 2016
 *
 * Do not share this file with anyone
 */
#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>

#include "lexer.h"
#include "inputbuf.h"

using namespace std;

string reserved[] = { "END_OF_FILE",
    "IF", "WHILE", "DO", "THEN", "PRINT",
    "PLUS", "MINUS", "DIV", "MULT",
    "EQUAL", "COLON", "COMMA", "SEMICOLON",
    "LBRAC", "RBRAC", "LPAREN", "RPAREN",
    "NOTEQUAL", "GREATER", "LESS", "LTEQ", "GTEQ",
    "DOT", "NUM", "ID", "ERROR", "REALNUM", "BASE08NUM", "BASE16NUM" // TODO: Add labels for new token types here (as string)
};

#define KEYWORDS_COUNT 5
string keyword[] = { "IF", "WHILE", "DO", "THEN", "PRINT" };

void Token::Print()
{
    cout << "{" << this->lexeme << " , "
         << reserved[(int) this->token_type] << " , "
         << this->line_no << "}\n";
}

LexicalAnalyzer::LexicalAnalyzer()
{
    this->line_no = 1;
    tmp.lexeme = "";
    tmp.line_no = 1;
    tmp.token_type = ERROR;
}

bool LexicalAnalyzer::SkipSpace()
{
    char c;
    bool space_encountered = false;

    input.GetChar(c);
    line_no += (c == '\n');

    while (!input.EndOfInput() && isspace(c)) {
        space_encountered = true;
        input.GetChar(c);
        line_no += (c == '\n');
    }

    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
    return space_encountered;
}

bool LexicalAnalyzer::IsKeyword(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return true;
        }
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s)
{
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return (TokenType) (i + 1);
        }
    }
    return ERROR;
}

bool isdigit08(vector<char> abc){
    for (int i=0; i<abc.size(); i++){
        if (abc[i]>='0' && abc[i]<='7'){
            continue;
        }
        else{
            return false;
        }
    }
    return true;
}

Token LexicalAnalyzer::ScanNumber(){
    char first, second, b, x, y, temp;
    vector<char> arr;
    vector<char> num;
    input.GetChar(first);
    input.GetChar(temp);
    if (isdigit(first)) {
        if (first=='0' && temp!='.' && temp!='x') {
            input.UngetChar(temp);
            tmp.lexeme+=first;
            tmp.token_type = NUM;
            tmp.line_no=line_no;
            return tmp;
        } 
        else {
            input.UngetChar(temp);
            tmp.lexeme = "";
            while (!input.EndOfInput() && isdigit(first)) {
                num.push_back(first);
                tmp.lexeme += first;
                input.GetChar(first);
            }
            tmp.token_type = NUM;
            if (!input.EndOfInput()) {
                input.UngetChar(first);
            }
        }
        input.GetChar(second);
        if (second=='.' && !input.EndOfInput()){
            input.GetChar(x);
            if (isdigit(x)){
                tmp.lexeme+=second;
                tmp.lexeme +=x;
                input.GetChar(y);
                while(!input.EndOfInput() && isdigit(y)) {
                    tmp.lexeme += y;
                    input.GetChar(y);
                }
                tmp.token_type=REALNUM;
                if (!input.EndOfInput()) {
                    input.UngetChar(y);
                }
            }
            else{
                input.UngetChar(x);
                input.UngetChar(second);
            }   
        }
        else if (second=='x' && !input.EndOfInput()){
            input.GetChar(x);
            input.GetChar(y);
            if (x=='0' && y=='8' && !input.EndOfInput()){
                if (isdigit08(num)){
                    tmp.token_type=BASE08NUM;
                    tmp.lexeme+=second;
                    tmp.lexeme+=x;
                    tmp.lexeme+=y;
                }
            }
            else if (x=='1' && y=='6' && !input.EndOfInput()){
                tmp.token_type=BASE16NUM;
                tmp.lexeme+=second;
                tmp.lexeme+=x;
                tmp.lexeme+=y;
            }
            else{
                input.UngetChar(y);
                input.UngetChar(x);
                input.UngetChar(second);
            }
        }
        else if ((isxdigit(second) && (isdigit(second)||isupper(second))) && !input.EndOfInput()){
            arr.push_back(second);
            input.GetChar(b);
            while(!input.EndOfInput() && (isxdigit(b) && (isdigit(b)||isupper(b)))) {
                arr.push_back(b);
                input.GetChar(b);
            }
            if (b=='x' && !input.EndOfInput()){
                arr.push_back(b);
                input.GetChar(x);
                input.GetChar(y);
                if (x=='1' && y=='6'){
                    tmp.token_type=BASE16NUM;
                    for (int i=0; i<arr.size(); i++){
                        tmp.lexeme+=arr[i];
                    }
                    tmp.lexeme+=x;
                    tmp.lexeme+=y;
                }
                else{
                    input.UngetChar(y);
                    input.UngetChar(x);
                    // input.UngetChar(b);
                    if(arr.size()>1){
                        for (int i=arr.size()-1; i>0; i--){
                            input.UngetChar(arr[i]);
                        }
                        input.UngetChar(second);
                    }
                    else{
                        input.UngetChar(second);
                    }
                }
            }
            else {
                input.UngetChar(b);
                if(arr.size()>1){
                    for (int i=arr.size()-1; i>0; i--){
                        input.UngetChar(arr[i]);
                    }
                    input.UngetChar(second);
                }
                else{
                    input.UngetChar(second);
                }
            }
        }
        else{
            input.UngetChar(second);
        }
        tmp.line_no = line_no;
        return tmp;
        arr.clear();
        num.clear();
    }
    else {
        if (!input.EndOfInput()) {
            input.UngetChar(second);
            input.UngetChar(first);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
        tmp.line_no = line_no;
        return tmp;
    }
}

Token LexicalAnalyzer::ScanIdOrKeyword()
{
    char c;
    input.GetChar(c);

    if (isalpha(c)) {
        tmp.lexeme = "";
        while (!input.EndOfInput() && isalnum(c)) {
            tmp.lexeme += c;
            input.GetChar(c);
        }
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.line_no = line_no;
        if (IsKeyword(tmp.lexeme))
            tmp.token_type = FindKeywordIndex(tmp.lexeme);
        else
            tmp.token_type = ID;
    } else {
        if (!input.EndOfInput()) {
            input.UngetChar(c);
        }
        tmp.lexeme = "";
        tmp.token_type = ERROR;
    }
    return tmp;
}

// you should unget tokens in the reverse order in which they
// are obtained. If you execute
//
//    t1 = lexer.GetToken();
//    t2 = lexer.GetToken();
//    t3 = lexer.GetToken();
//
// in this order, you should execute
//
//    lexer.UngetToken(t3);
//    lexer.UngetToken(t2);
//    lexer.UngetToken(t1);
//
// if you want to unget all three tokens. Note that it does not
// make sense to unget t1 without first ungetting t2 and t3
//
TokenType LexicalAnalyzer::UngetToken(Token tok)
{
    tokens.push_back(tok);;
    return tok.token_type;
}

Token LexicalAnalyzer::GetToken(){
    char c;

    // if there are tokens that were previously
    // stored due to UngetToken(), pop a token and
    // return it without reading from input
    if (!tokens.empty()) {
        tmp = tokens.back();
        tokens.pop_back();
        return tmp;
    }

    SkipSpace();
    tmp.lexeme = "";
    tmp.line_no = line_no;
    input.GetChar(c);
    switch (c) {
        case '.':
            tmp.token_type = DOT;
            return tmp;
        case '+':
            tmp.token_type = PLUS;
            return tmp;
        case '-':
            tmp.token_type = MINUS;
            return tmp;
        case '/':
            tmp.token_type = DIV;
            return tmp;
        case '*':
            tmp.token_type = MULT;
            return tmp;
        case '=':
            tmp.token_type = EQUAL;
            return tmp;
        case ':':
            tmp.token_type = COLON;
            return tmp;
        case ',':
            tmp.token_type = COMMA;
            return tmp;
        case ';':
            tmp.token_type = SEMICOLON;
            return tmp;
        case '[':
            tmp.token_type = LBRAC;
            return tmp;
        case ']':
            tmp.token_type = RBRAC;
            return tmp;
        case '(':
            tmp.token_type = LPAREN;
            return tmp;
        case ')':
            tmp.token_type = RPAREN;
            return tmp;
        case '<':
            input.GetChar(c);
            if (c == '=') {
                tmp.token_type = LTEQ;
            } else if (c == '>') {
                tmp.token_type = NOTEQUAL;
            } else {
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
                tmp.token_type = LESS;
            }
            return tmp;
        case '|':
            input.GetChar(c);
            
        case '>':
            input.GetChar(c);
            if (c == '=') {
                tmp.token_type = GTEQ;
            } else {
                if (!input.EndOfInput()) {
                    input.UngetChar(c);
                }
                tmp.token_type = GREATER;
            }
            return tmp;
        default:
            if (isdigit(c)) {
                input.UngetChar(c);
                return ScanNumber();
            } else if (isalpha(c)) {
                input.UngetChar(c);
                return ScanIdOrKeyword();
            } else if (input.EndOfInput())
                tmp.token_type = END_OF_FILE;
            else
                tmp.token_type = ERROR;

            return tmp;
    }
}

int main()
{
    LexicalAnalyzer lexer;
    Token token;

    token = lexer.GetToken();
    token.Print();
    while (token.token_type != END_OF_FILE)
    {
        token = lexer.GetToken();
        token.Print();
    }
}