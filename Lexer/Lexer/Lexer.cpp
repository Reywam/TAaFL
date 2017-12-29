#include "stdafx.h"

using namespace std;

const unsigned int MAX_STRING_SIZE = 255;
const unsigned int MAX_ID_SIZE = 255;

const string IF_STATEMENT = "if";
const string ELSE_STATEMENT = "else";
const string ASSIGNMENTT = "=";
const string ONE_LINE_COMMENT = "//";
const string OPEN_COMMENT = "/*";
const string CLOSE_COMMENT = "*/";
const string EOLN = "\n";

const string AND_STATEMENT = "and";
const string OR_STATEMENT = "or";
const string NOT_STATEMENT = "not";

const string ARRAY_OPEN_BRACKET = "[";
const string ARRAY_CLOSE_BRACKET = "]";

const string BLOCK_OPEN_BRACKET = "{";
const string BLOCK_CLOSE_BRACKET = "}";

const string SIMPLE_OPEN_BRACKET = "(";
const string SIMPLE_CLOSE_BRACKET = ")";

enum tokenType {
	ERROR
	, KEYWORD
	, NUMBER
	, IDENTIFIER
	, CONDITION_IF
	, CONDITION_ELSE
	, COMPARATOR
	, ASSIGNMENT
	, FIXED_FLOAT
	, FLOAT
	, STRING
	, DELIMITER
	, OPERATOR
	, CHAR
	, BOOLEAN
	, ARRAY_OPEN
	, ARRAY_CLOSE
	, BLOCK_OPEN
	, BLOCK_CLOSE
	, OPEN_BRACKET
	, CLOSE_BRACKET
	, LOGICAL_STATEMENT
};

const unordered_map<size_t, string> tokensNames = {
	{ERROR, "ERROR"}
	, {IDENTIFIER, "IDENTIFIER"}
	, {CONDITION_IF, "CONDITION_IF"}
	, {CONDITION_ELSE, "CONDITION_ELSE"}
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
	, {BLOCK_OPEN, "BLOCK_OPEN"}
	, {BLOCK_CLOSE, "BLOCK_CLOSE"}
	, {LOGICAL_STATEMENT, "LOGICAL_STATEMENT"}
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
	, "bool"
	, "float"
	, "return"
};

const set<string> BOOLEANS = {
	"true"
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

const set<string> LEX_DELIMITERS = {
	"."
	, ","
	, ";"
};

const set<char> DUMMY_CHARS = {
	' '
	, '\n'
	, '\t'
	, EOF
};

const set<string> DUMMY_LEX = {
	" "
	, "\n"
	, "\t"	
};

bool isBoolean(const string &lexeme) {
	return BOOLEANS.find(lexeme) != BOOLEANS.end();
}

bool isIdentifier(const string &lexeme) {
	bool result = true;

	if (isalpha(lexeme[0]) && lexeme.size() <= MAX_ID_SIZE) {
		for (size_t i = 0; i < lexeme.size(); i++) {
			if (!(isalpha(lexeme[i]) || isdigit(lexeme[i]))) {
				result = false;
				break;
			}
		}
	}
	else {
		result = false;
	}

	return result;
}

bool isKeyword(const string &lexeme) {
	return KEYWORDS.find(lexeme) != KEYWORDS.end();
}

bool isOperator(const string &lexeme) {
	return OPERATORS.find(lexeme) != OPERATORS.end();
}

bool isComparator(const string &lexeme) {
	return COMPARATORS.find(lexeme) != COMPARATORS.end();
}

bool isNumber(const string &lexeme) {
	bool result = true;

	size_t zerosCount = 0;

	if (lexeme == "") {
		return true;
	}

	if (lexeme == "0") {
		return true;
	}

	size_t i = 0;
	while (lexeme[i] == '0') {
		zerosCount++;
		i++;
	}

	if (zerosCount != 0 || lexeme.size() == 0) {
		return false;
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

	if (lexeme == ".") {
		return false;
	}

	if (pointPos < 0 || pointPos >= lexeme.size()) {
		return false;
	}

	string beforePointPart;	

	for (size_t i = 0; i < pointPos; i++) {
		beforePointPart += lexeme[i];
	}

	for (size_t i = pointPos + 1; i < lexeme.size(); i++) {
		if (!isdigit(lexeme[i])) {
			result = false;
			break;
		}
	}

	if (isNumber(beforePointPart) && result) {
		result = true;
	}
	else {
		result = false;
	}

	return result;
}

bool isExpFloat(const string &lexeme) {
	bool result = true;

	if (lexeme[0] != 'E') {
		return false;
	}

	if (result) {
		if (!(lexeme[1] == '-' || lexeme[1] == '+')) {
			return false;
		}

		for (size_t i = 2; i < lexeme.size(); i++) {
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
		return false;
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
	return LEX_DELIMITERS.find(lexeme) != LEX_DELIMITERS.end();
}

bool isString(const string &lexeme) {
	size_t start = lexeme.find_first_of('\\"');
	size_t end = lexeme.find_last_of('\\"');

	if (start == end) {
		return false;
	}	

	return true;
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
	for (size_t i = 0; i < bufferSize && !input.eof(); i++) {
		ch = input.get();
		if (!input.eof())
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

string ReadLexeme(ifstream &input) {
	char currentChar;
	string lexeme;

	while (!input.eof()) {

		currentChar = input.get();
		if (input.eof()) {			
			break;
		}

		if ((currentChar != '.' && !isdigit(currentChar) && !isalpha(currentChar)) && lexeme.empty()) {
		//if ((!isdigit(currentChar) && !isalpha(currentChar)) && lexeme.empty()) {
			lexeme += currentChar;
			break;
		}

		if (!input.eof() && (!isdigit(currentChar) && !isalpha(currentChar)) && currentChar != '.') {
		//if (!input.eof() && (!isdigit(currentChar) && !isalpha(currentChar))) {
			input.unget();
			break;
		}
		else {
			lexeme += currentChar;
		}
	}
	return lexeme;
}

string ReadLexeme(vector<char> &buffer, size_t &i) {
	string lexeme;

	char currentChar;
	char nextChar;

	for (; i < buffer.size(); i++) {
		currentChar = buffer[i];

		if ((!isdigit(currentChar) && !isalpha(currentChar)) && lexeme.empty()) {
			lexeme += currentChar;
			i++;
			break;
		}

		if ((!isdigit(currentChar) && !isalpha(currentChar))) {
			break;
		}
		else {
			lexeme += currentChar;
		}
	}
	return lexeme;
}

bool isPointlessFloat(const string &lexeme) {
	bool result = true;
	
	size_t expPos = lexeme.find('E');

	if (expPos >= lexeme.size() || expPos <= 0) {
		return false;
	}

	if (result) {
		for (size_t i = 0; i < expPos, i < lexeme.size(); i++) {
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

bool tryToMakeFloatWithoutPoint(string &lex, const vector<string> &lexemes, size_t &pos) {
	bool result = false;
	string startLex = lex;
	size_t i = 0;
	size_t movesCount = 4;
	string floatLex;
	try {
		for (; i < movesCount && i < lexemes.size() && pos < lexemes.size(); i++) {
			lex = lexemes[pos];
			if (DUMMY_CHARS.find(lex[0]) != DUMMY_CHARS.end()) {
				break;
			}
			floatLex += lex;
			pos++;
		}

		if (isPointlessFloat(floatLex)) {
			lex = floatLex;
			result = true;
			pos--;
		}
		else {
			throw exception();
		}
	}
	catch (const exception &ex) {
		lex = startLex;
		pos -= i;
	}

	return result;
}

bool tryToMakeFloat(string &lex, const vector<string> &lexemes, size_t &pos) {
	bool result = false;
	string startLex = lex;
	size_t i = 0;
	size_t movesCount = 3;
	string floatLex;
	try {
		for (; i < movesCount && i < lexemes.size() && pos < lexemes.size(); i++) {
			lex = lexemes[pos];
			if (DUMMY_CHARS.find(lex[0]) != DUMMY_CHARS.end()) {
				break;
			}
			floatLex += lex;
			pos++;
		}
		if (isFloat(floatLex) || isExpFloat(floatLex)) {
			lex = floatLex;
			result = true;
			pos--;
		}
		else {
			throw exception();
		}
	}
	catch (const exception &ex) {
		lex = startLex;
		pos -= i;
	}	
	return result;
}

bool tryToMakeFixedFloatWithoutZero(string &lex, const vector<string> &lexemes, size_t &pos) {	
	bool result = false;
	string startLex = lex;
	size_t i = 0;
	size_t movesCount = 2;
	string fixedFloatLeftLex;
	try {
		for (; i < movesCount && i < lexemes.size() && pos < lexemes.size(); i++) {
			lex = lexemes[pos];
			fixedFloatLeftLex += lex;
			pos++;
		}
		if (isFixedFloat(fixedFloatLeftLex)) {
			lex = fixedFloatLeftLex;
			result = true;
			pos--;
		}
		else {
			throw exception();
		}
	}
	catch (const exception &ex) {
		lex = startLex;
		pos -= i;
	}

	return result;
}

bool tryToMakeFixedFloat(string &lex, const vector<string> &lexemes, size_t &pos) {
	bool result = false;
	string startLex = lex;
	size_t i = 0;
	size_t movesCount = 3;
	string fixedFloatLex;	
	try {
		for (; i < movesCount && i < lexemes.size() && pos < lexemes.size(); i++) {
			lex = lexemes[pos];
			fixedFloatLex += lex;
			pos++;
		}
		if (isFixedFloat(fixedFloatLex)) {
			lex = fixedFloatLex;
			result = true;
			pos--;
		}
		else {
			throw exception();
		}
	}
	catch (const exception &ex) {
		lex = startLex;
		pos -= i;
	}

	return result;
}

bool isIfCondition(const string &lex) {
	return lex == IF_STATEMENT;
}

bool isElseCondition(const string &lex) {
	return lex == ELSE_STATEMENT;
}

bool isAssignment(const string &lex) {
	return lex == ASSIGNMENTT;
}

bool tryToMakeOneLineComment(string &lex, const vector<string> &lexemes, size_t &pos) {
	bool result = false;
	string startLex = lex;
	size_t i = 0;
	size_t movesCount = 2;
	string lineCommentLex;
	try {
		for (; i < movesCount && i < lexemes.size() && pos < lexemes.size(); i++, pos++) {
			lex = lexemes[pos];
			lineCommentLex += lex;
		}
		if (lineCommentLex == ONE_LINE_COMMENT) {
			result = true;
			pos--;
		}
		else {
			throw exception();
		}
	}
	catch (const exception &ex) {
		lex = startLex;
		pos -= i;
	}

	return result;
}

bool tryToMakeComment(string &lex, const vector<string> &lexemes, size_t &pos) {
	bool result = false;
	string startLex = lex;
	size_t i = 0;
	size_t movesCount = 2;
	string lineCommentLex;
	try {
		for (; i < movesCount && i < lexemes.size() && pos < lexemes.size(); i++, pos++) {
			lex = lexemes[pos];
			lineCommentLex += lex;
		}
		if (lineCommentLex == OPEN_COMMENT) {
			result = true;
		}
		else {
			throw exception();
		}
	}
	catch (const exception &ex) {
		lex = startLex;
		pos -= i;
	}

	return result;
}

void SkipLexeme(string &lex, const vector<string> &lexemes, size_t &pos) {
	pos++;
	if (pos < lexemes.size()) {
		lex = lexemes[pos];
	} 
	else {
		pos--;
	}
}

bool tryToMakeCloseComment(string &lex, const vector<string> &lexemes, size_t &pos) {
	bool result = false;
	string startLex = lex;
	size_t i = 0;
	size_t movesCount = 2;
	string lineCommentLex;
	try {
		for (; i < movesCount && i < lexemes.size() && pos < lexemes.size(); i++, pos++) {
			lex = lexemes[pos];
			lineCommentLex += lex;
		}
		if (lineCommentLex == CLOSE_COMMENT) {
			result = true;
		}
		else {
			throw exception();
		}
	}
	catch (const exception &ex) {
		lex = startLex;
		pos -= i;
	}

	return result;
}

bool tryToMakeDoubleComparator(string &lex, const vector<string> &lexemes, size_t &pos) {
	bool result = false;
	string startLex = lex;
	size_t i = 0;
	size_t movesCount = 2;
	string compLex;
	try {
		for (; i < movesCount && i < lexemes.size() && pos < lexemes.size(); i++, pos++) {
			lex = lexemes[pos];
			compLex += lex;
		}
		if (isComparator(compLex)) {
			lex = compLex;
			result = true;
			pos--;
		}
		else {
			throw exception();
		}
	}
	catch (const exception &ex) {
		lex = startLex;
		pos -= i;
	}

	return result;
}

string GetStringFromLex(const string &lex) {
	string str;
	if (lex == "\"\"") {
		str = "";
	}	
	else {
		str = lex.substr(1, lex.size() - 2);
	}
	return str;
}

bool tryToMakeString(string &lex, const vector<string> &lexemes, size_t &pos) {
	//Читать от кавычки до кавычки или конца строки
	bool result = false;
	string startLex = lex;
	string stringLex;
	size_t startPos = pos;
	try {
		if (lex != "\"") {
			throw exception();
		}
		pos++;
		stringLex += lex;

		for (; pos < lexemes.size()
			&& lexemes[pos] != EOLN
			&& lexemes[pos] != "\""
			; pos++) {
			lex = lexemes[pos];
			stringLex += lex;
		}
		if (pos < lexemes.size())
		{
			lex = lexemes[pos];
			stringLex += lex;
		}

		if (!isString(stringLex) && (lex == EOLN || pos >= lexemes.size())) {
			lex = stringLex;
			throw stringLex;
		}

		if ((stringLex == "" || isString(stringLex)) && GetStringFromLex(stringLex).size() <= MAX_STRING_SIZE) {
			lex = lex = GetStringFromLex(stringLex);
			result = true;
		}
		else {
			throw exception();
		}
	}
	catch (const string &ex) {
		pos = startPos;
	}
	catch (const exception &ex) {
		lex = startLex;
		pos = startPos;
	}

	return result;
}

bool isBraket(const string &lex, Token &tok) {
	bool result = true;
	if (lex == SIMPLE_OPEN_BRACKET) {
		tok = Token(OPEN_BRACKET, lex);
	}
	else if (lex == SIMPLE_CLOSE_BRACKET) {
		tok = Token(CLOSE_BRACKET, lex);
	}
	else if (lex == BLOCK_OPEN_BRACKET) {
		tok = Token(BLOCK_OPEN, lex);
	}
	else if (lex == BLOCK_CLOSE_BRACKET) {
		tok = Token(BLOCK_CLOSE, lex);
	}
	else if (lex == ARRAY_OPEN_BRACKET) {
		tok = Token(ARRAY_OPEN, lex);
	}
	else if (lex == ARRAY_CLOSE_BRACKET) {
		tok = Token(ARRAY_CLOSE, lex);
	}
	else {
		result = false;
	}

	return result;
}

bool isLogicalStatement(const string &lex) {
	return lex == AND_STATEMENT || lex == OR_STATEMENT || lex == NOT_STATEMENT;
}

bool isChar(string &lex) {
	bool result = false;

	if (lex.size() == 3 && lex[0] == '\'' && lex[2] == '\'') {
		lex = lex[1];
		result = true;
	}

	return result;
}

bool tryToMakeChar(string &lex, const vector<string> &lexemes, size_t &pos) {
	bool result = false;
	string startLex = lex;
	size_t i = 0;
	size_t movesCount = 3;
	string charLex;
	try {
		for (; i < movesCount && i < lexemes.size() && pos < lexemes.size(); i++, pos++) {
			lex = lexemes[pos];			
			if (lex != " " && DUMMY_CHARS.find(lex[0]) != DUMMY_CHARS.end()) {
				break;
			}
			charLex += lex;
		}
		if (isChar(charLex)) {
			lex = charLex;
			result = true;
			pos--;
		}
		else {
			throw exception();
		}
	}
	catch (const exception &ex) {
		lex = startLex;
		pos -= i;
	}

	return result;
}

bool isCharSequence(const string &lex) {
	bool result = true;
	for (char ch : lex) {
		if (!isalpha(ch)) {
			result = false;
			break;
		}
	}
	return result;
}

string CalculateFullLexInList(const vector<string> &lexemes, size_t &i) {
	string fullLex;
	string currentLex;
	for (; i < lexemes.size(); i++) {
		currentLex = lexemes[i];

		if (!isNumber(currentLex) 
			&& !isCharSequence(currentLex)
			&& lexemes.empty()
			&& currentLex != ".") {
			i--;
			break;
		}

		if ((!isNumber(currentLex) && !isCharSequence(currentLex)) && currentLex != ".") {
			i--;
			break;
		}
		else {
			fullLex += currentLex;
		}
	}

	if (fullLex == "") {
		fullLex = currentLex;
	}

	return fullLex;
}

Token CalculateToken(string &lex, const vector<string> &lexemes, size_t &pos) {
	
	// От частного к общему
	//lex = CalculateFullLexInList(lexemes, pos);
	Token tok = Token(ERROR, lex);

	if (tryToMakeString(lex, lexemes, pos)) {
		tok = Token(STRING, lex);
	}
	else if (tryToMakeChar(lex, lexemes, pos)) {
		tok = Token(CHAR, lex);
	}
	else if (tryToMakeFloat(lex, lexemes, pos)) {
		tok = Token(FLOAT, lex);
	}
	else if (tryToMakeFloatWithoutPoint(lex, lexemes, pos)) {
		tok = Token(FLOAT, lex);
	}
	/*else if (tryToMakeFixedFloat(lex, lexemes, pos)) {
		tok = Token(FLOAT, lex);
	}*/
	else if (isFixedFloat(lex)) {
		tok = Token(FIXED_FLOAT, lex);
	}	
	else if (tryToMakeFixedFloatWithoutZero(lex, lexemes, pos)) {
		tok = Token(FIXED_FLOAT, lex);
	}	
	else if (isNumber(lex)) {
		tok = Token(NUMBER, lex);
	}
	else if (isKeyword(lex)) {
		tok = Token(KEYWORD, lex);
	}
	else if (isBoolean(lex)) {
		tok = Token(BOOLEAN, lex);
	}
	else if (isIfCondition(lex)) {
		tok = Token(CONDITION_IF, lex);
	}
	else if (isElseCondition(lex)) {
		tok = Token(CONDITION_ELSE, lex);
	}
	else if (isLogicalStatement(lex)) {
		tok = Token(LOGICAL_STATEMENT, lex);
	}
	else if (isIdentifier(lex)) {
		tok = Token(IDENTIFIER, lex);
	}
	else if (isOperator(lex)) {
		tok = Token(OPERATOR, lex);
	}
	else if (tryToMakeDoubleComparator(lex, lexemes, pos)) {
		tok = Token(COMPARATOR, lex);
	}
	else if (isComparator(lex)) {
		tok = Token(COMPARATOR, lex);
	}
	else if (isAssignment(lex)) {
		tok = Token(ASSIGNMENT, lex);
	}
	else if (isDelimiter(lex)) {
		tok = Token(DELIMITER, lex);
	}
	else {
		isBraket(lex, tok);
	}

	return tok;
}


void SkipComment(string &currLex, size_t &i, const vector<string> &lexemes) {	
	while (!tryToMakeCloseComment(currLex, lexemes, i) && i < lexemes.size() - 1) {
		SkipLexeme(currLex, lexemes, i);
	}	
	if (i < lexemes.size() - 1)
		i--;
}

void SkipLineComment(string &currLex, size_t &i, const vector<string> &lexemes) {
	while (currLex != EOLN && i < lexemes.size() - 1) {
		SkipLexeme(currLex, lexemes, i);
	}
	if (i < lexemes.size() - 1)
		currLex = lexemes[i];
}

void ProcessLexemesList(const vector<string> &lexemes) {
	
	for (size_t i = 0; i < lexemes.size(); i++) {
		string currLex = lexemes[i];
		
		if (tryToMakeOneLineComment(currLex, lexemes, i)) {			
			SkipLineComment(currLex, i, lexemes);
			continue;
		}

		if (tryToMakeComment(currLex, lexemes, i)) {
			SkipComment(currLex, i, lexemes);
			continue;
		}		
		if (DUMMY_LEX.find(currLex) == DUMMY_LEX.end() && (i < lexemes.size()))
		{			
			Token tok = CalculateToken(currLex, lexemes, i);
			cout << tok.toString() << endl;
		}
	}
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

	vector<string> lexemes;

	string lex;
	bool end = false;
	while (!end) {
		lex = ReadLexeme(input);

		if (lex != "") {
			lexemes.push_back(lex);
		}
		if (input.eof()) {
			end = true;
			break;
		}
		
	}

	ProcessLexemesList(lexemes);

	return 0;
}