#pragma once
#include <string>
#include <variant>
#include <iostream>

struct SourceLocation {
	int line = 1;
	int col = 1;
};

enum class TokenType {
	KW_INT, KW_FLOAT, KW_STRING,

	KW_WHILE, KW_IF, KW_ELSE, KW_SWITCH, KW_CASE, KW_DEFAULT, KW_BREAK,

	KW_INPUT, KW_OUTPUT,

	OP_AND, OP_OR, OP_NOT,

	OP_LT,       // <
	OP_GT,       // >
	OP_LE,       // <=
	OP_GE,       // >=
	OP_EQ,       // ==
	OP_NE,       // != or <>

	OP_ASSIGN,   // =
	OP_PLUS,     // +
	OP_MINUS,    // -
	OP_MUL,      // *
	OP_DIV,      // /

	SEMICOLON,   // ;
	COMMA,       // ,
	COLON,       // :
	LPAREN,      // (
	RPAREN,      // )
	LBRACE,      // {
	RBRACE,      // }

	IDENTIFIER,
	CONST_INT,
	CONST_FLOAT,
	CONST_STRING,

	END_OF_FILE,
	UNKNOWN_ERROR
};

using LiteralValue = std::variant<std::monostate, int, double, std::string>;

struct Token {
	TokenType type;
	std::string text;
	LiteralValue value;
	SourceLocation loc;

	void print() const {
		std::cout << "[" << loc.line << ":" << loc.col << "] Token: "
			<< static_cast<int>(type) << " ('" << text << "')\n";
	}
};

