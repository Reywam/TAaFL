#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <regex>

using namespace std;

enum states {
	OK
	, NOT_OK
};

enum tokenTipe {
	ERROR
	, IDENTIFIER
	, CONDITION
	, KEYWORD
	, COMPARATOR
	, ASSIGNMENT
	, NUMBER
	, FIXED_FLOAT
	, FLOAT
	, STRING
	, DELIMITER
	, OPERATOR
	, OPEN_BRAKET
	, CLOSE_BRAKET
	, CHAR
	, BOOLEAN
	, ARRAY_OPEN
	, ARRAY_CLOSE
};

const regex ID_PATTERN("([a-z]|[A-Z])+[0-9]*");

const regex NUMBER_PATTERN("[0-9]+");
const regex FIXED_FLOAT_PATTERN("[0-9]+\\.[0-9]+");
const regex FLOAT_PATTERN("[0-9]+\\.[0-9]+[0-9]*[E][+|-][0-9]+");

const regex DELIMITER_PATTERN(";|\n");
const regex OPERATOR_PATTERN("\\+|-|\\*|\\/");
const regex ASSIGNMENT_PATTERN("=");
const regex COMPARATOR_PATTERN("<=|>=|==|!=|>|<");
const regex CONDITION_PATTERN("if|else");

const regex OPEN_BRAKET_PATTERN("[(]|[{]");
const regex CLOSE_BRAKET_PATTERN("[)]|[}]");

const regex OPEN_ARRAY_BRAKET_PATTERN("[\\[]");
const regex CLOSE_ARRAY_BRAKET_PATTERN("[\\]]");

const regex CHAR_PATTERN("['][a-z|A-Z][']");
const regex BOOLEAN_PATTERN("true|false");

class Token{
	size_t id;
	string lexeme;
public:
	Token() {		
	}

	Token(size_t id, string lexeme) {
		this->id = id;
		this->lexeme = lexeme;
	}

	size_t getId() {
		return id;
	}

	string getLex() {
		return lexeme;
	}

	string idToString() {
		string result;
		switch (id) {
			case ERROR:
				result = "ERROR";
				break;
			case IDENTIFIER:
				result = "IDENTIFIER";
				break;
			case CONDITION:
				result = "CONDITION";
				break;
			case KEYWORD:
				result = "KEYWORD";
				break;
			case COMPARATOR:
				result = "COMPARATOR";
				break;
			case ASSIGNMENT:
				result = "ASSIGNMENT";
				break;
			case NUMBER:
				result = "NUMBER";
				break;
			case FIXED_FLOAT:
				result = "FIXED_FLOAT";
				break;
			case FLOAT:
				result = "FLOAT";
				break;
			case STRING:
				result = "STRING";
				break;
			case DELIMITER:
				result = "DELIMITER";
				break;
			case OPERATOR:
				result = "OPERATOR";
				break;
			case OPEN_BRAKET:
				result = "OPEN_BRAKET";
				break;
			case CLOSE_BRAKET:
				result = "CLOSE_BRAKET";
				break;
			case BOOLEAN:
				result = "BOOLEAN";
				break;
			case CHAR:
				result = "CHAR";
				break;
			case ARRAY_OPEN:
				result = "ARRAY_OPEN";
				break;
			case ARRAY_CLOSE:
				result = "ARRAY_CLOSE";
				break;
		}
		return result;		
	}

	string toString()
	{
		return idToString() + "->" + lexeme;
	}
};

const set<string> KEYWORDS = {	
	"for"
	, "string"
	, "int"
	, "char"
	, "boolean"
	, "float"
	, "true"
	, "false"
};

const set<char> DELIMITERS = {
	','	
	, ';'
	, ' '
	, '\n'	
	, ')'
	, '('
	, '{'
	, '}'
	, '['
	, ']'
};

const set<char> OPERATORS = {
	'+'
	, '-'
	, '*'
	, '/'
};

const set<string> COMPARATORS = {
	"=="
	, "!="
	, ">"
	, "<"
	, ">="
	, "<="
};

const set<char> ASSIGNMENTS = {
	'='
};

const set<char> BRAKETS = {
	'('
	, ')'
	, '{'
	, '}'
};

string ReadProgrammCodeToString(ifstream &input) {
	string codeString;
	string inputLine;

	while (getline(input, inputLine)) {
		codeString += inputLine;
		codeString += "\n";
	}

	return codeString;
}

void SkipComment(size_t &pos, const string &str) {
	for (;pos < str.size(); pos++) {
		if (str[pos] == '/' && str[pos - 1] == '*') {
			//pos++;
			break;
		}
	}
}

Token GetToken(const string &lexeme) {
	// Проверка лексемы по регуляркам
	Token tok;
	
	if (regex_match(lexeme, ID_PATTERN)) {
		if (KEYWORDS.find(lexeme) != KEYWORDS.end()) {
			// Ключевое слово
			tok = Token(KEYWORD, lexeme);
		} else if (regex_match(lexeme, CONDITION_PATTERN)) {
			tok = Token(CONDITION, lexeme);
		} else  {
			// Идентификатор
			tok = Token(IDENTIFIER, lexeme);
		}
	} else if(regex_match(lexeme, NUMBER_PATTERN)) {
		// Целое число
		tok = Token(NUMBER, lexeme);	
	} else if (regex_match(lexeme, FIXED_FLOAT_PATTERN)) {
		// Число с фикс.точкой
		tok = Token(FIXED_FLOAT, lexeme);
	} else if (regex_match(lexeme, FLOAT_PATTERN)) {
		// Float
		tok = Token(FLOAT, lexeme);
	} else if (regex_match(lexeme, DELIMITER_PATTERN)) {
		// Разделители
		tok = Token(DELIMITER, lexeme);
	} else if (regex_match(lexeme, OPERATOR_PATTERN)) {
		tok = Token(OPERATOR, lexeme);
	} else if (regex_match(lexeme, ASSIGNMENT_PATTERN)) {
		tok = Token(ASSIGNMENT, lexeme);
	} else if (regex_match(lexeme, COMPARATOR_PATTERN)) {
		tok = Token(COMPARATOR, lexeme);
	} else if (regex_match(lexeme, OPEN_BRAKET_PATTERN)) {
		tok = Token(OPEN_BRAKET, lexeme);
	} else if (regex_match(lexeme, CLOSE_BRAKET_PATTERN)) {
		tok = Token(CLOSE_BRAKET, lexeme);
	} else if (regex_match(lexeme, OPEN_ARRAY_BRAKET_PATTERN)) {
		tok = Token(ARRAY_OPEN, lexeme);
	} else if (regex_match(lexeme, CLOSE_ARRAY_BRAKET_PATTERN)) {
		tok = Token(ARRAY_CLOSE, lexeme);
	} else if (regex_match(lexeme, CHAR_PATTERN)) {
		tok = Token(CHAR, lexeme);
	} else {
		tok = Token(ERROR, lexeme);
	}

	return tok;
}

bool isIdentidier(const string &lexeme) {
	bool result = false;
	size_t state;
	if (isalpha(lexeme[0])) {		
		state = OK;
		for (size_t i = 0; i < lexeme.size(); i++) {
			result = true;
			if (state == states::NOT_OK) {
				result = false;
				break;
			}

			char currenChar = lexeme[i];
			if (isalpha(currenChar) || isdigit(currenChar)) {
				state = states::OK;
			}
		}
	}	
	
	return result;
}

bool isKeyword(const string &lexeme) {
	bool result = false;

	if (KEYWORDS.find(lexeme) != KEYWORDS.end()) {
		result = true;
	}

	return result;
}

bool isCondition(const string &lexeme) {
	bool result = false;

	if (lexeme == "if" || "else") {
		result = true;
	}

	return result;
}

size_t ProcessLexeme(const string &lexeme) {
	size_t type = tokenTipe::ERROR;

	if (isCondition(lexeme)) {
		type = CONDITION;
		cout << "___CONDITION" << endl;
	} else if (isKeyword(lexeme)) {
		type = KEYWORD;
		cout << "___KEYWORD" << endl;
	} else if (isIdentidier(lexeme)) {
		type = IDENTIFIER;
		cout << "___IDENTIFIER" << endl;
	}

	return type;
}

int main(int argc, char* argv[])
{
	ifstream input;
	
	if (argc != 2) {
		cout << "Choose input file." << endl;
		return 1;
	}

	input = ifstream(argv[1]);

	string codeString = ReadProgrammCodeToString(input);	

	//ProcessLexeme("if");

	string lexeme;
	for (size_t i = 0; i < codeString.size(); i++) {
		char currentChar = codeString[i];
		if (currentChar == '*' && lexeme == "/") {
			lexeme = "";
			currentChar = '\n';
			SkipComment(i, codeString);
		}
		if (DELIMITERS.find(currentChar) != DELIMITERS.end() && lexeme == "" 
			&& currentChar != ' '
			&& currentChar != '\n') {
			lexeme += currentChar;
			Token tok; 
			tok = GetToken(lexeme);
			lexeme = "";
			cout << tok.toString() << endl;
		} else if (DELIMITERS.find(currentChar) != DELIMITERS.end() && lexeme != "") {
			Token tok;
			tok = GetToken(lexeme);
			lexeme = "";
			lexeme += currentChar;
			cout << tok.toString() << endl;
			if (currentChar != ' ' && currentChar != '\n') {
				tok = GetToken(lexeme);
				cout << tok.toString() << endl;				
			}
			lexeme = "";
			
		} else if (currentChar != ' ' && currentChar != '\n') {
			lexeme += currentChar;
		}
	}

    return 0;
}