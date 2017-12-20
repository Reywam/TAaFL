#include "stdafx.h"

using namespace std;

const unsigned int BUFFER_LENGTH = 1024;

enum tokenType {
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
	, OPEN_BRACKET
	, CLOSE_BRACKET
	, CHAR
	, BOOLEAN
	, ARRAY_OPEN
	, ARRAY_CLOSE
};

const unordered_map<size_t, string> tokensNames = {
	{ERROR, "ERROR"}
	, {IDENTIFIER, "IDENTIFIER"}
	, {CONDITION, "CONDITION"}
	, {KEYWORD, "KEYWORD"}
	, {COMPARATOR, "COMPARATOR"}
	, {ASSIGNMENT, "ASSIGNMENT"}
	, {NUMBER, "NUMBER"}
	, {FIXED_FLOAT, "FIXED_FLOAT"}
	, {FLOAT, "FLOAT"}
	, {STRING, "STRING"}
	, {DELIMITER, "DELIMITER"}
	, {OPERATOR, "OPERATOR"}
	, {OPEN_BRACKET, "OPEN_BRACKET"}
	, {CLOSE_BRACKET, "CLOSE_BRACKET"}
	, {CHAR, "CHAR"}
	, {BOOLEAN, "BOOLEAN"}
	, {ARRAY_OPEN, "ARRAY_OPEN"}
	, {ARRAY_CLOSE, "ARRAY_CLOSE"}
};

class Token {
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
		return tokensNames.at(id);
	}

	string toString() {
		return idToString() + "->" + lexeme;
	}
};

const set<string> KEYWORDS = {	
	"for"
	, "const"
	, "class"
	, "public"
	, "private"
	, "string"
	, "int"
	, "char"
	, "boolean"
	, "float"
	, "true"
	, "false"
	, "return"
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

const set<string> DELIMITERS = {
	","
	, "."
	, ";"
	, "\n"
	, "(", ")"
	, "[", "]"
	, "{", "}"
	, "+", "-", "*", "/"
	, "="
	, "<", ">", ">=", "<=", "==", "!="
};

const set<char> IGNORED_SEPARATORS = {
	' '
	, '\n'
	, ')'
	, '}'
	, ']'
	, ';'
};

const set<char> DUMMY_CHARS = {
	' '
	, '\n'
	, '\t'
	, EOF
};

void SkipComment(size_t &pos, const vector<char> &str) {
	for (;pos < str.size(); pos++) {
		if (pos != 0 && str[pos] == '/' && str[pos - 1] == '*' || (str[pos] == EOF && str[pos] != 'я')) {
			break;
		}
	}
}

void SkipLineComment(size_t &pos, const vector<char> &str) {
	for (; pos < str.size(); pos++) {		
		if (str[pos] == '\n' || (str[pos] == EOF && str[pos] != 'я')) {
			break;
		}
	}
}

bool isIdentifier(const string &lexeme) {
	bool result = true;

	if (isalpha(lexeme[0])) {
		for (size_t i = 0; i < lexeme.size(); i++) {						
			if (!(isalpha(lexeme[i]) || isdigit(lexeme[i]))) {
				result = false;
				break;
			}
		}
	} else {
		result = false;
	}
	
	return result;
}

bool isKeyword(const string &lexeme) {
	return KEYWORDS.find(lexeme) != KEYWORDS.end();
}

bool isCondition(const string &lexeme) {
	return (lexeme == "if" || lexeme == "else");
}

bool isOperator(const string &lexeme) {
	return OPERATORS.find(lexeme) != OPERATORS.end();
}

bool isComparator(const string &lexeme) {
	return COMPARATORS.find(lexeme) != COMPARATORS.end();
}

bool isNumber(string &lexeme) {
	bool result = true;
	
	while (lexeme[0] == '0') {
		lexeme.erase(0, 1);
	}

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

	if (pointPos >= lexeme.size() || pointPos < 0) {
		return false;
	}

	if (expPos >= lexeme.size() || expPos <= 0) {
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
	return DELIMITERS.find(lexeme) != DELIMITERS.end();
}

bool isCharSequence(string &lexeme) {
	size_t start = lexeme.find_first_of("\'");
	size_t end = lexeme.find_last_of("\'");

	if (start == end) {
		return false;
	}

	if (lexeme == "''") {
		lexeme = "";
	} else if (start != end) {
		lexeme == lexeme.substr(start + 1, end - 1);
	}
	
	return true;
}

bool isString(string &lexeme) {
	size_t start = lexeme.find_first_of('\\"');
	size_t end = lexeme.find_last_of('\\"');

	if (start == end) {
		return false;
	}

	if (lexeme == "\"\"") {
		lexeme = "";
	}
	else if (start != end) {
		lexeme == lexeme.substr(start + 1, end - 1);
	}

	return true;
}

void ProcessLexeme(string &lexeme, vector<string> &ids) {

	Token tok = Token(ERROR, lexeme);
	if (isCondition(lexeme)) {
		tok = Token(CONDITION, lexeme);		
	} else if (isKeyword(lexeme)) {
		tok = Token(KEYWORD, lexeme);
	} else if (isIdentifier(lexeme)) {
		tok = Token(IDENTIFIER, lexeme);
		ids.push_back(lexeme);
	} else if (isOperator(lexeme)) {
		tok = Token(OPERATOR, lexeme);
	} else if (isComparator(lexeme)) {
		tok = Token(COMPARATOR, lexeme);
	} else if (isNumber(lexeme)) {
		tok = Token(NUMBER, lexeme);
	} else if (isCharSequence(lexeme)) {
		tok = Token(CHAR, lexeme);
	} else if (isString(lexeme)) {
		tok = Token(STRING, lexeme);
	} else if (isFixedFloat(lexeme)) {
		tok = Token(FIXED_FLOAT, lexeme);
	} else if (isFloat(lexeme)) {
		tok = Token(FLOAT, lexeme);
	} else if (lexeme == "=") {
		tok = Token(ASSIGNMENT, lexeme);
	} else if (lexeme == "(") {
		tok = Token(OPEN_BRACKET, lexeme);
	} else if (lexeme == ")") {
		tok = Token(CLOSE_BRACKET, lexeme);
	} else if (lexeme == "[") {
		tok = Token(ARRAY_OPEN, lexeme);
	} else if (lexeme == "]") {
		tok = Token(ARRAY_CLOSE, lexeme);
	} else if (lexeme == "{") {
		tok = Token(OPEN_BRACKET, lexeme);
	} else if (lexeme == "}") {
		tok = Token(CLOSE_BRACKET, lexeme);
	} else if (isDelimiter(lexeme)) {
		tok = Token(DELIMITER, lexeme);
	}
	
	cout << tok.toString() << endl;	
}

string ReadString(size_t &pos, const vector<char> &str) {
	string lexeme;
	lexeme += str[pos];
	pos++;

	for (; pos < str.size(); pos++) {
		if (str[pos] == '"' || str[pos] == EOF || str[pos] == '\t' || str[pos] == '\n') {
			break;
		}
		lexeme += str[pos];
	}

	if (str[pos] == '"') {
		lexeme += '"';
	}

	return lexeme;
}

void ReadDataToBuffer(ifstream &input, vector<char> &buffer, size_t bufferSize) {
	char ch = NULL;
	for (size_t i = 0; i < bufferSize && (int)input.tellg() != EOF; i++) {
		ch = input.get();
		buffer.push_back(ch);
	}
}

string ReadCharSequence(size_t &pos, const vector<char> &buffer) {
	string lexeme;

	lexeme += buffer[pos];
	pos++;
	for (; pos < buffer.size(); pos++) {
		if (buffer[pos] == '\'' || buffer[pos] == EOF || buffer[pos] == '\n') {
			break;
		}
		lexeme += buffer[pos];
	}
	
	if (buffer[pos] == '\'') {
		lexeme += '\'';
	}

	return lexeme;
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		cout << "Choose input file." << endl;
		return 1;
	}

	ifstream input = ifstream(argv[1]);
	if (!input.is_open()) {
		cout << "Failed to open file " << argv[1] << endl;
		return 1;
	}

	vector<char> buffer;
	
	vector<string> ids;

	ReadDataToBuffer(input, buffer, BUFFER_LENGTH);	
	string lexeme;
	for (size_t i = 0; i < buffer.size() && buffer[i] != 'я' && buffer[i] != EOF; i++) {
		if (i == buffer.size()) {
			auto lastElem = buffer.back();
			buffer.clear();			
			buffer.push_back(lastElem);
			i = 0;
			ReadDataToBuffer(input, buffer, BUFFER_LENGTH);
		}

		char currentChar = buffer[i];
		char nextChar = buffer[i + 1];

		// Проверка символов-пустышек, их игнорим
		if (DUMMY_CHARS.find(currentChar) != DUMMY_CHARS.end()) {
			if (lexeme != "") {
				ProcessLexeme(lexeme, ids);
				lexeme = "";
			}
			continue;
		}

		if (currentChar == '/' && nextChar == '*') {
			SkipComment(i, buffer);
			continue;
		}

		if (currentChar == '/' && nextChar == '/') {
			SkipLineComment(i, buffer);
			continue;
		}

		// Проверка компараторов
		if ((currentChar == '='
			|| currentChar == '!'
			|| currentChar == '<'
			|| currentChar == '>') && nextChar == '=') {
			lexeme += currentChar;
			lexeme += nextChar;
			i++;
			Token tok = Token(COMPARATOR, lexeme);
			cout << tok.toString() << endl;
			lexeme = "";
			continue;
		}

		// Проверка что это char
		if (currentChar == '\'') {
			lexeme = ReadCharSequence(i, buffer);
			ProcessLexeme(lexeme, ids);			
			lexeme = "";
			continue;
		}

		// String
		if (currentChar == '"') {
			lexeme = ReadString(i, buffer);
			ProcessLexeme(lexeme, ids);
			lexeme = "";
			continue;
		}
	

		// Float
		if ((isdigit(currentChar) && nextChar == '.')
			|| (currentChar == '.' && isdigit(nextChar))) {
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

		// Обработка символа-небуквы
		if (!(isdigit(currentChar) || isalpha(currentChar))
			&& !(isdigit(nextChar) || isalpha(nextChar))) {
			lexeme += currentChar;
			ProcessLexeme(lexeme, ids);
			lexeme = "";			
			continue;
		}

		if (isdigit(currentChar) || isalpha(currentChar)) {
			lexeme += currentChar;
		} else if (!(isdigit(nextChar) || isalpha(nextChar))
			&& IGNORED_SEPARATORS.find(nextChar) == IGNORED_SEPARATORS.end()) {
			lexeme += currentChar;
			lexeme += nextChar;
			ProcessLexeme(lexeme, ids);
			i++;
			lexeme = "";			
		} else {
			lexeme += currentChar;
			ProcessLexeme(lexeme, ids);
			lexeme = "";			
		}

		if ((isdigit(currentChar) || isalpha(currentChar))
			&& !(isdigit(nextChar) || isalpha(nextChar))) {
			ProcessLexeme(lexeme, ids);
			lexeme = "";			
		}
	}

	if (lexeme != "") {
		ProcessLexeme(lexeme, ids);
	}

	cout << "IDS:" << endl;
	for (auto &id : ids) {
		cout << id << endl;
	}

    return 0;
}