#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <set>
#include <string>
#include <regex>

using namespace std;

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
};

const regex ID_PATTERN("([a-z]|[A-Z])+[0-9]*");

const regex NUMBER_PATTERN("[0-9]+");
const regex FIXED_FLOAT_PATTERN("[0-9]+\\.[0-9]+");
const regex FLOAT_PATTERN("[0-9]+\\.[0-9]+[0-9]*[E][+|-][0-9]+");

const regex DELIMITER_PATTERN(";| |\n");
const regex OPERATOR_PATTERN("\\+|-|\\*|\\/");
const regex ASSIGNMENT_PATTERN("=");
const regex COMPARATOR_PATTERN("<=|>=|==|!=|>|<");
const regex CONDITION_PATTERN("if|else");

const regex OPEN_BRAKET_PATTERN("[(]|[{]");
const regex CLOSE_BRAKET_PATTERN("[)]|[}]");

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
	, "bool"
	, "float"
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
	} else {
		tok = Token(ERROR, lexeme);
	}

	return tok;
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

	string lexeme;	
	for (size_t i = 0; i < codeString.size(); i++) {
		char currentChar = codeString[i];		
		
		if (DELIMITERS.find(currentChar) != DELIMITERS.end() && lexeme == "") {
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
			tok = GetToken(lexeme);
			cout << tok.toString() << endl;
			lexeme = "";
		} else {
			lexeme += currentChar;
		}
	}

    return 0;
}