#include <unordered_map>
#include <iostream>
#include <string>
#include <vector>
#include <cstring>
#include "parser.h"
#include "lexer.h"
#include "inputbuf.h"

using namespace std;

int enumType;
int enumCount = 4;

std::unordered_map<std::string, Symbol*> symbolTable; 

std::vector <std::string> varlist;

void Parser::addSymbol(const std::string& name, int line, int type) {
    if (symbolTable.find(name) != symbolTable.end()) {
        syntax_error();
        return;
    }
	varlist.push_back(name);
    symbolTable[name] = new Symbol(line, name, type);
	if(type==1){
		symbolTable[name]->out="int";
	}
	else if(type==2){
		symbolTable[name]->out="real";
	}
	else if(type==3){
		symbolTable[name]->out="bool";
	}
}

int Parser::lookupSymbol(const std::string& name) {
    if (symbolTable.empty()){
		addSymbol(name, token.line_no, enumCount);
        enumCount++; 
        return 4; 
	}
	else{
		auto it = symbolTable.find(name);
		if (it != symbolTable.end()) {
			return it->second->data_type;
		} 
		else {
			addSymbol(name, token.line_no, enumCount);
			enumCount++; 
			return enumCount-1; 
		}
	}
}

void Parser::output(){
	string print = "";
	int s=varlist.size();
	for (int i=0; i<s; i++){
		string temp=varlist[i];
		int type=symbolTable[temp]->data_type;
		if (type==1){
			symbolTable[temp]->out="int";
		}
		else if(type==2){
			symbolTable[temp]->out="real";
		}
		else if(type==3){
			symbolTable[temp]->out="bool";
		}
		
		if(symbolTable[temp]->printed==false && symbolTable[temp]->data_type<4){
			print+=temp;
			print += ": " + symbolTable[temp]->out + " #\n";
			symbolTable[temp]->printed=true;
		}
		else if(symbolTable[temp]->printed==false && (symbolTable[temp]->data_type>3 || symbolTable[temp]->data_type==0)){
			int j=i;
			print+=temp;
			symbolTable[temp]->printed=true;
			for (j; j<s; j++){
				string temp2=varlist[j];
				if (symbolTable[temp2]->data_type==symbolTable[temp]->data_type && temp2!=temp){
					print += ", " + temp2;
					symbolTable[temp2]->printed=true;
				}
			}
			print = print + ": ? #\n";
		}
	}
	cout <<print;
}

void Parser::syntax_error(){
	cout << "\nSyntax Error\n";
	exit(1);
}

int Parser::parse_program(){ 
	token = lexer.GetToken();
	while(token.token_type!=END_OF_FILE){
		if(token.token_type == ID){
			lexer.UngetToken(token);
			parse_globalVars();
			parse_body();
		}
		else if(token.token_type == LBRACE){
			lexer.UngetToken(token);
			parse_body();
		}
		else{
			syntax_error();
			exit(1);
		}
		token=lexer.GetToken();
	}
	return 0;
}

int Parser::parse_globalVars(){ 
	parse_vardecllist();
	return 0;
}

int Parser::parse_vardecllist(){

	token = lexer.GetToken();
	while(token.token_type == ID){
		lexer.UngetToken(token);
		parse_vardecl();
		token = lexer.GetToken();
	}
	lexer.UngetToken(token);

	return 0;
}

int Parser::parse_vardecl(){ 

	token = lexer.GetToken();
	if(token.token_type != ID){
		syntax_error();
		exit(1);
	}
	lexer.UngetToken(token);
	parse_varlist();
	token = lexer.GetToken();
	if(token.token_type != COLON){
		syntax_error();
		exit(1);
		return 0;
	}
	token = lexer.GetToken();
	if(token.token_type == INT || token.token_type == REAL || token.token_type == BOO){
		lexer.UngetToken(token);
		parse_typename();
		token = lexer.GetToken();
		if(token.token_type != SEMICOLON){
			syntax_error();
			exit(1);
			return 0;
		}
	}
	else{
		syntax_error();
		exit(1);
	}

	return 0;
}

int Parser::parse_varlist(){ 

	token = lexer.GetToken();
	char* lexeme = (char*)malloc(sizeof(token.lexeme)+1);
    memcpy(lexeme, (token.lexeme).c_str(), (token.lexeme).size()+1);
    addSymbol(lexeme, token.line_no, 0);
	int temp;
	if(token.token_type != ID){
		syntax_error();
		exit(1);
	}
	else{
		Token t2 = lexer.GetToken();
		if(t2.token_type == COMMA){
			temp=parse_varlist();
		}
		else if (t2.token_type==COLON){
			lexer.UngetToken(t2);
		}
		else{
			syntax_error();
			exit(1);		
		}
	}

	return 0;
}

int Parser::parse_typename(){ 
	token = lexer.GetToken();
	int ln=token.line_no;
	int dt=token.token_type;
	for (auto& pair : symbolTable) {
        Symbol* sym = pair.second;
        if (sym->line_no == ln) {
            sym->data_type = dt;
        }  
	}
	return 0;
}

int Parser::parse_body(){ 
	token = lexer.GetToken();
	if(token.token_type == LBRACE){
		parse_stmtlist();
		token = lexer.GetToken();
		if(token.token_type != RBRACE){
			syntax_error();
			exit(1);
		}
	}
	else{
		syntax_error();
		exit(1);
	}

	return 0;
}

int Parser::parse_stmtlist(){ 
	token = lexer.GetToken();
	while(token.token_type == ID || token.token_type == IF || token.token_type == WHILE || token.token_type == SWITCH){
		lexer.UngetToken(token);
		parse_stmt();
		token = lexer.GetToken();
	}
	lexer.UngetToken(token);

	return 0;
}

int Parser::parse_stmt(){
	token = lexer.GetToken();
	if(token.token_type == ID){
		lexer.UngetToken(token);
		parse_assstmt();
	}
	else if(token.token_type == IF){
		lexer.UngetToken(token);
		parse_ifstmt();
	}
	else if(token.token_type == WHILE){
		lexer.UngetToken(token);
		parse_whilestmt();
	}
	else if(token.token_type == SWITCH){
		lexer.UngetToken(token);
		parse_switchstmt();
	}
	else{
		syntax_error();
		exit(1);
	}
	return 0;
}

void Parser::Type_change(int ct, int nt) {
    for (auto& pair : symbolTable) {
        Symbol* sym = pair.second;
        if (sym->data_type == ct) {
            sym->data_type = nt;
        }
    }
}

int Parser::parse_assstmt(){ 
	int tempI;    
    string name;
    int LHS;
    int RHS;
	token = lexer.GetToken();
	if(token.token_type != ID){
		syntax_error();
		exit(1);
	}
	LHS = lookupSymbol(token.lexeme);
	token = lexer.GetToken();
	if(token.token_type != EQUAL){
		syntax_error();
		exit(1);
	}
	token = lexer.GetToken();
	if(token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM || token.token_type == TR || token.token_type == FA || token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT || token.token_type == DIV || token.token_type == LESS || token.token_type == GREATER || token.token_type == GTEQ || token.token_type == LTEQ || token.token_type == EQUAL || token.token_type == NOTEQUAL || token.token_type == NOT){
		lexer.UngetToken(token);
		RHS = parse_expression();
		if(LHS == 1 || LHS == 2 || LHS == 3){
			if(LHS!=RHS){
				if(LHS < 3){
					cout << "TYPE MISMATCH " << token.line_no << " C1" << endl;
					exit(1);
				}
				else{
					Type_change(RHS,LHS);
					RHS = LHS;
				}
			}
		}
		else{
			Type_change(LHS,RHS);
			LHS = RHS;
		}
		token = lexer.GetToken();
		if(token.token_type != SEMICOLON){
			syntax_error();
			exit(1);
		}
	}
	else{
		syntax_error();
		exit(1);
	}
	return 0;
}

int Parser::parse_expression(){ 
	int tempI;
    token = lexer.GetToken();
	if(token.token_type == NOT){
		tempTokenType = lexer.UngetToken(token);
        tempI = parse_unaryOperator();
        tempI = parse_expression();
        if(tempI != 3){
            cout << "TYPE MISMATCH " << token.line_no << " C3" <<endl;
            exit(1);
        }
	}
	else if(token.token_type == PLUS || token.token_type == MINUS || token.token_type == MULT ||  token.token_type == DIV || token.token_type == GREATER || token.token_type == LESS || token.token_type == GTEQ || token.token_type == LTEQ || token.token_type == EQUAL || token.token_type == NOTEQUAL){
		int leftExp;
        int rightExp;
        tempTokenType = lexer.UngetToken(token);
        tempI = parse_binaryOperator(); 
        leftExp = parse_expression();
        rightExp = parse_expression();     
        if((leftExp != rightExp) || (tempI<=14 && tempI>=24 && tempI!=26)){
            if(tempI == 15 || tempI == 16 || tempI == 17 || tempI == 18){
                if(leftExp <= 2 && rightExp > 3){
                    Type_change(rightExp, leftExp);
                    rightExp = leftExp;
                }
                else if(leftExp > 3 && rightExp <= 2){
                    Type_change(rightExp, leftExp);
                    leftExp = rightExp;
                }
                else if(leftExp > 3 && rightExp > 3){
                    Type_change(rightExp, leftExp);
                    rightExp = leftExp;
                }
                else{
                    cout << "TYPE MISMATCH " << token.line_no << " C2"<<endl;
                    exit(1);
                }
            }
            else if(tempI == 19 || tempI == 20 || tempI == 21 || tempI == 22 || tempI == 23 || tempI == 26){
                if(rightExp > 3 && leftExp > 3){
                    Type_change(rightExp, leftExp);
                    rightExp = leftExp;
                    return(3);
                }
                else{
                    cout << "TYPE MISMATCH " << token.line_no << " C2"<<endl;
                    exit(1);
                }
            }
            else{
                cout << "TYPE MISMATCH " << token.line_no << " C2"<<endl;
                exit(1);
            }
        }
        if(tempI == 19 || tempI == 20 || tempI == 21 || tempI == 23 || tempI == 26){
            tempI = 3;
        }
        else{
            tempI = rightExp;
        }
	}
	else if(token.token_type == ID || token.token_type == NUM || token.token_type == REALNUM || token.token_type == TR || token.token_type == FA){
		lexer.UngetToken(token);
		tempI=parse_primary();
	}
	else{
		syntax_error();
        exit(1);
	}
	return tempI;
}

int Parser::parse_unaryOperator(){
	token = lexer.GetToken();
	if(token.token_type != NOT){
		syntax_error();
		exit(1);
	}
	else{
		return(1);
	}
	return 0;
}

int Parser::parse_binaryOperator(){

	token = lexer.GetToken();
    if(token.token_type == PLUS){
        return(15);
    }
    else if(token.token_type == MINUS){
        return(16);
    }
    else if(token.token_type == MULT){
        return(17);
    }
    else if(token.token_type == DIV){
        return(18);
    }
    else if(token.token_type == GREATER){
        return(20);
    }
    else if(token.token_type == LESS){
        return(23);
    }
    else if(token.token_type == GTEQ){
        return(19);
    }
    else if(token.token_type == LTEQ){
        return(21);
    }
    else if(token.token_type == EQUAL ){
        return(26);
    }
    else if(token.token_type == NOTEQUAL){
        return(22);
    }
    else{
        syntax_error();
		exit(1);
        return(-1);
    }
}

int Parser::parse_primary(){
	token = lexer.GetToken();
    if(token.token_type == ID){
        return(lookupSymbol(token.lexeme));
    }
    else if(token.token_type == NUM ){
        return(1);
    }
    else if(token.token_type == REALNUM){
        return(2);
    }
    else if(token.token_type == TR ){
        return(3);
    }
    else if(token.token_type == FA){
        return(3);
    }
	else{
		syntax_error();
		exit(1);
	}
	return 0;
}

int Parser::parse_ifstmt(){
	int tempI;
	token = lexer.GetToken();
	if(token.token_type != IF){
		syntax_error();
		exit(1);
	}
	token = lexer.GetToken();
	if(token.token_type != LPAREN){
		syntax_error();
		exit(1);
	}
	tempI = parse_expression();
	if(tempI != 3){
		cout<< "TYPE MISMATCH " << token.line_no << " C4" << endl;
		exit(1);
	}
	token = lexer.GetToken();
	if(token.token_type != RPAREN){
		syntax_error();
		exit(1);
	}
	parse_body();
	return 0;
}

int Parser::parse_whilestmt(){
	int tempI;
	token = lexer.GetToken();
	if(token.token_type != WHILE){
		syntax_error();
		exit(1);
	}
	token = lexer.GetToken();
	if(token.token_type != LPAREN){
		syntax_error();
		exit(1);
	}
	tempI=parse_expression();
	if(tempI != 3){
		cout<< "TYPE MISMATCH " << token.line_no << " C4" << endl;
		exit(1);
	}
	token = lexer.GetToken();
	if(token.token_type != RPAREN){
		syntax_error();
		exit(1);
	}
	parse_body();

	return 0;
}

int Parser::parse_switchstmt(){
	int tempI;
	token = lexer.GetToken();
	if(token.token_type != SWITCH){
		syntax_error();
		exit(1);
	}
	token = lexer.GetToken();
	if(token.token_type != LPAREN){
		syntax_error();
		exit(1);
	}
	tempI = parse_expression();
	if(tempI <= 3 && tempI != 1){
		cout<< "TYPE MISMATCH " << token.line_no << " C5"<<endl;
		exit(1);
	}
	token = lexer.GetToken();
	if(token.token_type != RPAREN){
		syntax_error();
		exit(1);
	}
	token = lexer.GetToken();
	if(token.token_type != LBRACE){
		syntax_error();
		exit(1);
	}
	parse_caselist();
	token = lexer.GetToken();
	if(token.token_type != RBRACE){
		syntax_error();
		exit(1);
	}
	return 0;
}

int Parser::parse_caselist(){
	token = lexer.GetToken();
	if(token.token_type == CASE){
		while(token.token_type == CASE){
			lexer.UngetToken(token);
			parse_case();
			token = lexer.GetToken();
		}
		lexer.UngetToken(token);
	}
	else{
		syntax_error();
		exit(1);
	}
	return 0;
}

int Parser::parse_case(){
	token = lexer.GetToken();
	if(token.token_type != CASE){
		syntax_error();
		exit(1);
	}
	token = lexer.GetToken();
	if(token.token_type != NUM){
		syntax_error();
		exit(1);
	}
	token = lexer.GetToken();
	if(token.token_type != COLON){
		syntax_error();
		exit(1);
	}
	parse_body();

	return 0;
}

int main(){
	int i;
    	Parser* parseProgram = new Parser();
    	i = parseProgram->parse_program();
		parseProgram->output();
	return 0;
}
