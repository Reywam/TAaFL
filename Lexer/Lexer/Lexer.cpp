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
	, CHAR
	, BOOLEAN
	, ARRAY_OPEN
	, ARRAY_CLOSE
};

const regex FIXED_FLOAT_PATTERN("[0-9]+\\.[0-9]+");
const regex FLOAT_PATTERN("[0-9]+\\.[0-9]+[0-9]*[E][+|-][0-9]+");

const regex ASSIGNMENT_PATTERN("=");
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

const set<string> OPERATORS = {
	"+"
	, "-"
	, "*"
	, "/"
};

const set<string> COMPARATORS = {
	"=="
	, "!="
	, ">"
	, "<"
	, ">="
	, "<="
};

const set<string> ASSIGNMENTS = {
	"="
};

const set<string> BRAKETS = {
	"("
	, ")"
	, "{"
	, "}"
};

const set<string> DELIMITERS = {
	","
	, ";"
	, "\n"
	, "(", ")"
	, "[", "]"
	, "{", "}"
	, "+", "-", "*", "/"
	, "="
	, "<", ">", ">=", "<=", "==", "!="
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
			break;
		}
	}
}

bool isIdentifier(const string &lexeme) {
	bool result = false;

	if (isalpha(lexeme[0])) {				
		for (size_t i = 0; i < lexeme.size(); i++) {
			result = false;

			char currenChar = lexeme[i];
			if (isalpha(currenChar) || isdigit(currenChar)) {
				result = true;
			}
			else {
				break;
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

	if (lexeme == "if" || lexeme == "else") {
		result = true;
	}

	return result;
}

bool isOperator(const string &lexeme) {
	bool result = true;

	if (OPERATORS.find(lexeme) == OPERATORS.end()) {
		result = false;
	}

	return result;
}

bool isComparator(const string &lexeme) {
	bool result = true;

	if (COMPARATORS.find(lexeme) == COMPARATORS.end()) {
		result = false;
	}

	return result;
}

bool isNumber(const string &lexeme) {
	bool result = true;

	for (size_t i = 0; i < lexeme.size(); i++) {
		if (!isdigit(lexeme[i])) {
			result = false;
			break;
		}
	}

	return result;
}

bool isFixedFloat(const string &lexeme) {
	bool result = true;
	size_t pointPos = lexeme.find_first_of('.');

	for (size_t i = 0; i < pointPos; i++) {
		if (!isdigit(lexeme[i])) {
			result = false;
			break;
		}
	}

	if (result) {
		for (size_t i = pointPos + 1; i < lexeme.size(); i++) {
			if (!isdigit(lexeme[i])) {
				result = false;
				break;
			}
		}
	}
	return result;
}

bool isFloat(const string &lexeme) {
	bool result = true;

	size_t pointPos = lexeme.find('.');
	size_t expPos = lexeme.find('E');

	if (pointPos >= lexeme.size() && pointPos <= 0) {
		return false;
	}

	if (expPos >= lexeme.size() && expPos <= 0) {
		return false;;
	}

	for (size_t i = 0; i < pointPos; i++) {
		if (!isdigit(lexeme[i])) {
			result = false;
			break;
		}
	}

	if (result) {
		for (size_t i = pointPos + 1; i < expPos; i++) {
			if (!isdigit(lexeme[i])) {
				result = false;
				break;
			}
		}
	}

	if (result) {
		if (!(lexeme[expPos + 1] == '-' || lexeme[expPos + 1] == '+')) {
			return false;
		}

		for (size_t i = expPos + 2; expPos + 2 < lexeme.size(), i < lexeme.size(); i++) {
			if (!isdigit(lexeme[i])) {
				result = false;
				break;
			}
		}
	}

	return result;
}

bool isDelimiter(const string &lexeme) {
	bool result = false;

	if (lexeme == ";") {
		result = true;
	}

	return result;
}

size_t ProcessLexeme(const string &lexeme) {

	Token tok = Token(ERROR, lexeme);

	if (isCondition(lexeme)) {
		tok = Token(CONDITION, lexeme);		
	} else if (isKeyword(lexeme)) {
		tok = Token(KEYWORD, lexeme);
	} else if (isIdentifier(lexeme)) {
		tok = Token(IDENTIFIER, lexeme);
	} else if (isOperator(lexeme)) {
		tok = Token(OPERATOR, lexeme);
	} else if (isComparator(lexeme)) {
		tok = Token(COMPARATOR, lexeme);
	} else if (isNumber(lexeme)) {
		tok = Token(NUMBER, lexeme);
	} else if (isFixedFloat(lexeme)) {
		tok = Token(FIXED_FLOAT, lexeme);		
	} else if (isFloat(lexeme)) {
		tok = Token(FLOAT, lexeme);
	} else if (isDelimiter(lexeme)) {
		tok = Token(DELIMITER, lexeme);
	}

	cout << tok.toString() << endl;

	return 0;
}

int main(int argc, char* argv[])
{		
	if (argc != 2) {
		cout << "Choose input file." << endl;
		return 1;
	}

	ifstream input = ifstream(argv[1]);

	string codeString = ReadProgrammCodeToString(input);	

	string lexeme;
	for (size_t i = 0; i < codeString.size(); i++) {
		char currentChar = codeString[i];
		char nextChar = codeString[i + 1];

		if (currentChar == ' ' || currentChar == '\n') {
			continue;
		}

		if (currentChar == '/' && nextChar == '*') {
			SkipComment(i, codeString);
			continue;
		}

		if (isdigit(currentChar) && nextChar == '.') {
			lexeme += currentChar;
			lexeme += nextChar;
			i++;
			continue;
		}

		if (currentChar == 'E' && (nextChar == '-' || nextChar == '+')) {
			lexeme += currentChar;
			lexeme += nextChar;
			i++;
			continue;
		}

		if (isdigit(currentChar) || isalpha(currentChar)) {
			lexeme += currentChar;
		} else if (!(isdigit(nextChar) || isalpha(nextChar)) && nextChar != ' ' && nextChar != '\n') {
			lexeme += currentChar;
			lexeme += nextChar;
			ProcessLexeme(lexeme);
			i++;
			lexeme = "";
		} else {
			lexeme += currentChar;
			ProcessLexeme(lexeme);
			lexeme = "";
		}

		if ((isdigit(currentChar) || isalpha(currentChar))
			&& !(isdigit(nextChar) || isalpha(nextChar))) {
			ProcessLexeme(lexeme);
			lexeme = "";
		}
	}

    return 0;
}