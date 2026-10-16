#include "Lexer.h"
#include <cctype>

Lexer::Lexer(std::string source)
	: src(std::move(source)) {
	initKeywords();
}

void Lexer::initKeywords() {
    keywords["int"] = TokenType::KW_INT;
    keywords["float"] = TokenType::KW_FLOAT;
    keywords["string"] = TokenType::KW_STRING;
    keywords["while"] = TokenType::KW_WHILE;
    keywords["if"] = TokenType::KW_IF;
    keywords["else"] = TokenType::KW_ELSE;
    keywords["switch"] = TokenType::KW_SWITCH;
    keywords["case"] = TokenType::KW_CASE;
    keywords["default"] = TokenType::KW_DEFAULT;
    keywords["break"] = TokenType::KW_BREAK;
    keywords["input"] = TokenType::KW_INPUT;
    keywords["output"] = TokenType::KW_OUTPUT;
    keywords["and"] = TokenType::OP_AND;
    keywords["or"] = TokenType::OP_OR;
    keywords["not"] = TokenType::OP_NOT;
}

char Lexer::peek() const {
    if (isAtEnd()) return '\0';
    return src[cursor];
}

char Lexer::get() {
    if (isAtEnd()) return '\0';
    char c = src[cursor++];
    if (c == '\n') {
        currentLine++;
        currentCol = 1;
    }
    else {
        currentCol++;
    }
    return c;
}

bool Lexer::isAtEnd() const {
    return cursor >= src.length();
}

void Lexer::addError(const std::string& msg, int line, int col) {
    errors.push_back("[" + std::to_string(line) + ":" + std::to_string(col) + "] Lexical error: " + msg);
}

std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (true) {
        Token tok = nextToken();
        tokens.push_back(tok);
        if (tok.type == TokenType::END_OF_FILE) break;
    }
    return tokens;
}

Token Lexer::nextToken() {
    DFAState state = DFAState::START;
    std::string buffer;
    SourceLocation tokenLoc;
    int safety = 0;

    while (!isAtEnd() || state != DFAState::START) {

        if (++safety > 100000) {
            std::cerr << "[FATAL] Nghi ngo infinite loop! cursor="
                << cursor << " state=" << (int)state
                << " c=" << (int)(unsigned char)peek() << "\n";
            return { TokenType::UNKNOWN_ERROR, "LOOP", {}, {currentLine, currentCol} };
        }
        char c = peek();

        switch (state) {
            case DFAState::START: {
                if (c == '\0') {
                    return { TokenType::END_OF_FILE, "", {}, {currentLine, currentCol} };
                }
                if (std::isspace(c)) {
                    get();
                    continue;
                }

                tokenLoc = { currentLine, currentCol };

                if (std::isalpha(c) || c == '_') {
                    buffer += get();
                    state = DFAState::IN_ID_OR_KEYWORD;
                }
                else if (std::isdigit(c)) {
                    buffer += get();
                    state = DFAState::IN_INT;
                }
                else if (c == '"') {
                    get();
                    state = DFAState::IN_STRING;
                }
                else if (c == '=') {
                    buffer += get();
                    state = DFAState::IN_ASSIGN_OR_EQ;
                }
                else if (c == '!') {
                    buffer += get();
                    state = DFAState::IN_EXCLAMATION;
                }
                else if (c == '<') {
                    buffer += get();
                    state = DFAState::IN_LESS;
                }
                else if (c == '>') {
                    buffer += get();
                    state = DFAState::IN_GREATER;
                }
                else if (c == '/') {
                    get();
                    state = DFAState::IN_SLASH;
                }
                
                else if (c == '+') { get(); return { TokenType::OP_PLUS, "+", {}, tokenLoc }; }
                else if (c == '-') { get(); return { TokenType::OP_MINUS, "-", {}, tokenLoc }; }
                else if (c == '*') { get(); return { TokenType::OP_MUL, "*", {}, tokenLoc }; }
                else if (c == ';') { get(); return { TokenType::SEMICOLON, ";", {}, tokenLoc }; }
                else if (c == ',') { get(); return { TokenType::COMMA, ",", {}, tokenLoc }; }
                else if (c == ':') { get(); return { TokenType::COLON, ":", {}, tokenLoc }; }
                else if (c == '(') { get(); return { TokenType::LPAREN, "(", {}, tokenLoc }; }
                else if (c == ')') { get(); return { TokenType::RPAREN, ")", {}, tokenLoc }; }
                else if (c == '{') { get(); return { TokenType::LBRACE, "{", {}, tokenLoc }; }
                else if (c == '}') { get(); return { TokenType::RBRACE, "}", {}, tokenLoc }; }
                else {
                    char errChar = get();
                    addError(std::string("Ký tự không hợp lệ '") + errChar + "'", tokenLoc.line, tokenLoc.col);
                    return { TokenType::UNKNOWN_ERROR, std::string(1, errChar), {}, tokenLoc };
                }
                break;
            }

            case DFAState::IN_ID_OR_KEYWORD: {
                if (std::isalnum(c) || c == '_') {
                    buffer += get();
                }
                else {
                    auto it = keywords.find(buffer);
                    if (it != keywords.end()) {
                        return { it->second, buffer, {}, tokenLoc };
                    }
                    return { TokenType::IDENTIFIER, buffer, {}, tokenLoc };
                }
                break;
            }
            
            case DFAState::IN_INT: {
                if (std::isdigit(c)) {
                    buffer += get();
                }
                else if (c == '.') {
                    buffer += get();
                    state = DFAState::IN_FLOAT_DOT;
                }
                else {
                    return { TokenType::CONST_INT, buffer, std::stoi(buffer), tokenLoc };
                }
                break;
            }

            case DFAState::IN_FLOAT_DOT: {
                if (std::isdigit(c)) {
                    buffer += get();
                    state = DFAState::IN_FLOAT;
                }
                else {
                    addError("	Invalid real constant format after the decimal point", tokenLoc.line, tokenLoc.col);
                    return { TokenType::UNKNOWN_ERROR, buffer, {}, tokenLoc };
                }
                break;
            }
                
            case DFAState::IN_FLOAT: {
                if (std::isdigit(c)) {
                    buffer += get();
                }
                else {
                    return{ TokenType::CONST_FLOAT, buffer, std::stod(buffer), tokenLoc };
                }
                break;
            }
            
            case DFAState::IN_STRING: {
                if (c == '\0') {
                    addError("String not closed with double quotes", tokenLoc.line, tokenLoc.col);
                    return { TokenType::UNKNOWN_ERROR, buffer, buffer, tokenLoc };
                }
                else if (c == '\\') {
                    get();
                    state = DFAState::IN_STRING_ESCAPE;
                }
                else if (c == '"') {
                    get();
                    return { TokenType::CONST_STRING, "\"" + buffer + "\"", buffer, tokenLoc };
                }
                else {
                    buffer += get();
                }
                break;
            }
            
            case DFAState::IN_STRING_ESCAPE: {
                if (c == '\0') {   
                    addError("String not closed with double quotes", tokenLoc.line, tokenLoc.col);
                    return { TokenType::UNKNOWN_ERROR, buffer, {}, tokenLoc };
                }
                if (c == 'n') buffer += '\n';
                else if (c == 't') buffer += '\t';
                else if (c == '"') buffer += '"';
                else if (c == '\\') buffer += '\\';
                else buffer += c;
                get();
                state = DFAState::IN_STRING;
                break;
            }

            case DFAState::IN_ASSIGN_OR_EQ: {
                if (c == '=') {
                    buffer += get();
                    return { TokenType::OP_EQ, buffer, {}, tokenLoc };
                }
                return { TokenType::OP_ASSIGN, buffer, {}, tokenLoc };
            }

            case DFAState::IN_EXCLAMATION: {
                if (c == '=') {
                    buffer += get();
                    return { TokenType::OP_NE, buffer, {}, tokenLoc };
                }
                return { TokenType::OP_NOT, buffer, {}, tokenLoc };
            }

            case DFAState::IN_LESS: {
                if (c == '=') {
                    buffer += get();
                    return { TokenType::OP_LE, buffer, {}, tokenLoc };
                }
                else if (c == '>') { 
                    buffer += get();
                    return { TokenType::OP_NE, buffer, {}, tokenLoc };
                }
                return { TokenType::OP_LT, buffer, {}, tokenLoc };
            }

            case DFAState::IN_SLASH: {
                if (c == '/') {
                    get();
                    state = DFAState::IN_LINE_COMMENT;
                }
                else if (c == '*') {
                    get();
                    state = DFAState::IN_BLOCK_COMMENT;
                }
                else {
                    return { TokenType::OP_DIV, "/", {}, tokenLoc };
                }
                break;
            }

            case DFAState::IN_LINE_COMMENT: {
                if (c == '\n' || c == '\0') {
                    state = DFAState::START;
                }
                else {
                    get();
                }
                break;
            }

            case DFAState::IN_BLOCK_COMMENT: {
                if (c == '\0') {
                    addError("Unclosed block comment (missing */)", tokenLoc.line, tokenLoc.col);
                    return { TokenType::UNKNOWN_ERROR, "", {}, tokenLoc };
                }
                else if (c == '*') {
                    get();
                    state = DFAState::IN_BLOCK_COMMENT_STAR;
                }
                else {
                    get();
                }
                break;
            }

            case DFAState::IN_BLOCK_COMMENT_STAR: {
                if (c == '/') {
                    get();
                    state = DFAState::START;
                }
                else if (c != '*') {
                    get();
                    state = DFAState::IN_BLOCK_COMMENT;
                }
                else {
                    get();
                }
                break;
            }
        }
    }
    return { TokenType::END_OF_FILE, "", {}, {currentLine, currentCol} };
}