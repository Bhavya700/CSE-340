#include <iostream>
#include <istream>
#include <vector>
#include <string>
#include <cctype>
#include "lexer.h"
#include "inputbuf.h"
using namespace std;

string reserved[] = { "END_OF_FILE",
    "EQUAL", "COLON", "COMMA", "SEMICOLON",
    "NUM", "ID", "ERROR", "LBRACE", "RBRACE", "PUBLIC", "PRIVATE"
};

#define KEYWORDS_COUNT 2
string keyword[] = { "public", "private" };

void Token::Print(){
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

bool LexicalAnalyzer::SkipSpace(){
    char c;
    bool space_encountered = false;
    input.GetChar(c);
    line_no += (c == '\n');
    while (!input.EndOfInput() && (isspace(c))) {
        space_encountered = true;
        input.GetChar(c);
        line_no += (c == '\n');
    }
    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
    return space_encountered;
}

bool LexicalAnalyzer::SkipComment(){
    char c;
    input.GetChar(c); 
    while (!input.EndOfInput() && c!='\n'){
        input.GetChar(c);
    }
    line_no++;
    if (!input.EndOfInput()) {
        input.UngetChar(c);
    }
}

bool LexicalAnalyzer::IsKeyword(string s){
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return true;
        }
    }
    return false;
}

TokenType LexicalAnalyzer::FindKeywordIndex(string s){
    for (int i = 0; i < KEYWORDS_COUNT; i++) {
        if (s == keyword[i]) {
            return (TokenType) (i + 1);
        }
    }
    return ERROR;
}

Token LexicalAnalyzer::ScanIdOrKeyword(){
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

TokenType LexicalAnalyzer::UngetToken(Token tok){
    tokens.push_back(tok);;
    return tok.token_type;
}

Token LexicalAnalyzer::GetToken(){
    char c;
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
        case '/':
            char c1;
            input.GetChar(c1);
            if (c1=='/'){
                SkipComment();
                return GetToken();
            }
            else{
                input.UngetChar(c1);
            }
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
        
        case '{':
            tmp.token_type = LBRACE;
            return tmp;
        case '}':
            tmp.token_type = RBRACE;
            return tmp;
        case 'p':
            char c2; char c3; char c4; char c5; char c6;
            input.GetChar(c2);
            input.GetChar(c3);
            input.GetChar(c4); 
            input.GetChar(c5); 
            input.GetChar(c6);
            if (c2=='u' && c3=='b' && c4=='l' && c5=='i' && c6=='c'){
                tmp.token_type=PUBLIC;
                return tmp;
            }
            else if(c2=='r' && c3=='i' && c4=='v' && c5=='a' && c6=='t'){
                char c7;
                input.GetChar(c7);
                if (c7=='e'){
                    tmp.token_type=PRIVATE;
                    return tmp;
                }
                else{
                    input.UngetChar(c7);
                    input.UngetChar(c6);
                    input.UngetChar(c5);
                    input.UngetChar(c4);
                    input.UngetChar(c3);
                    input.UngetChar(c2);
                }
            } 
            else{
                input.UngetChar(c6);
                input.UngetChar(c5);
                input.UngetChar(c4);
                input.UngetChar(c3);
                input.UngetChar(c2);
            }

        default:
            if (isdigit(c)){
                input.UngetChar(c);
            }
            else if (isalpha(c)) {
                input.UngetChar(c);
                return ScanIdOrKeyword();
            } 
            else if (input.EndOfInput())
                tmp.token_type = END_OF_FILE;
            else
                tmp.token_type = ERROR;
            return tmp;
    }
}
