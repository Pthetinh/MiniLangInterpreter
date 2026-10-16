#pragma once
#include "Common.h"
#include <string>
#include <vector>
#include <unordered_map>

enum class DFAState {
    START,
    IN_ID_OR_KEYWORD,
    IN_INT,
    IN_FLOAT_DOT,
    IN_FLOAT,
    IN_STRING,
    IN_STRING_ESCAPE,
    IN_ASSIGN_OR_EQ,   // = or ==
    IN_EXCLAMATION,    // ! or !=
    IN_LESS,           // <, <= or <>
    IN_GREATER,        // >, >=
    IN_SLASH,          // / or start comment //, /*
    IN_LINE_COMMENT,   // // ...
    IN_BLOCK_COMMENT,  // /* ...
    IN_BLOCK_COMMENT_STAR // /* ... *
};

class Lexer {
public:
    explicit Lexer(std::string source);

    std::vector<Token> tokenize();

    Token nextToken();

    const std::vector<std::string>& getErrors() const { return errors; }

private:
    std::string src;
    size_t cursor = 0;
    int currentLine = 1;
    int currentCol = 1;

    std::unordered_map<std::string, TokenType> keywords;
    std::vector<std::string> errors;

    void initKeywords();
    char peek() const;
    char get();
    bool isAtEnd() const;
    void addError(const std::string& msg, int line, int col);
};