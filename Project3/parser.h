
#ifndef __PARSER__H__
#define __PARSER__H__

#include <string>
#include <unordered_map>
#include <vector>
#include <cstring>
#include "inputbuf.h"
#include "lexer.h"

struct Symbol {
    int line_no;
    std::string variable_name;
    int data_type; 
    bool printed;
	std::string out;

    Symbol(int ln, const std::string& name, int type)
        : line_no(ln), variable_name(name), data_type(type), printed(false), out("") {}
};

class Parser {
	public:
		LexicalAnalyzer lexer;
		Token token;
		TokenType tempTokenType;
		int parse_program();
        void addSymbol(const std::string& name, int line, int type); 
        int lookupSymbol(const std::string& name); 
        void syntax_error();
		void output();
	private:
		int parse_varlist();
		int parse_unaryOperator();
		int parse_binaryOperator();
		int parse_primary();
		int parse_expression();
		int parse_assstmt();
		int parse_case();
		int parse_caselist();
		int parse_switchstmt();
		int parse_whilestmt();
		int parse_ifstmt();
		int parse_stmt();
		int parse_stmtlist();
		int parse_body();
		int parse_typename();
		int parse_vardecl();
		int parse_vardecllist();
		int parse_globalVars();
		void Type_change(int ct, int nt);
};

#endif  //__PARSER__H__

