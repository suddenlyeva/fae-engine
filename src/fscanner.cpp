#pragma once

#include "fscanner.hpp"
#include <cctype>

namespace fae
{
	//
	// Skip whitespace and comments
	void fScanner::skip()
	{
		// Whitespace characters
		while (*current == 13 || *current == 10 || *current == 9 || *current == 32
			// Comment characters
			|| *current == '#' || (*current == '/' && (current[1] == '/' || current[1] == '*'))) {

			//
			// For Comments
			if (*current == '#' || (*current == '/' && (current[1] == '/' || current[1] == '*'))) {

				// Single Line Comments
				if (*current == '#' || current[1] == '/') {
					do {
						++current;
					} while (*current != '\n' && *current != '\r');
				}
				else { // Multi-line Comments

					current += 2; // -> "/*"

					while (*current != '*' || current[1] != '/') {

						// Count lines
						if (*current == '\n') {
							++line;
						}
						++current;
					}

					current += 2; // -> "*/"
				}
			}
			// Count newlines
			else if (*current == '\n') {
				++line;
				++current;
			}
			else { // Is normal whitespace
				++current;
			}
		}
	}

	//
	// Scan next token
	void fScanner::advance()
	{
		// Skip whitespace first
		skip();

		// Null Terminator
		if (*current == 0) {
			next = _END;
			return;
		}

		// Long case for token
		switch (*current)
		{
			//
			// Bracket Types
		case '[':
			next = _OPEN_BRA;
			++current;
			break;

		case ']':
			next = _CLOSE_BRA;
			++current;
			break;

		case '(':
			next = _OPEN_PAR;
			++current;
			break;

		case ')':
			next = _CLOSE_PAR;
			++current;
			break;

		case '{':
			next = _OPEN_CUR;
			++current;
			break;

		case '}':
			next = _CLOSE_CUR;
			++current;
			break;

			//
			// Punctuation Types
		case '@':
			next = _AT;
			++current;
			break;

		case ',':
			next = _COMMA;
			++current;
			break;

		case ':':
			next = _COLON;
			++current;
			break;

		case ';':
			next = _SEMICOLON;
			++current;
			break;

			//
			// Operation Types
		case '~':
			next = _TILDE;
			++current;

			if (*current == '=') {
				next = _LINK_ASSIGN;
				++current;
			}
			break;

		case '*':
			next = _ASTERISK;
			++current;

			if (*current == '=') {
				next = _MULT_ASSIGN;
				++current;
			}
			break;

		case '/':
			next = _SLASH;
			++current;

			if (*current == '=') {
				next = _DIV_ASSIGN;
				++current;
			}
			break;

		case '%':
			next = _PERCENT;
			++current;

			if (*current == '=') {
				next = _MOD_ASSIGN;
				++current;
			}
			break;

		case '^':
			next = _CARET;
			++current;

			if (*current == '=') {
				next = _POW_ASSIGN;
				++current;
			}
			break;

			//
			// Logic and Equals
		case '=':
			next = _ASSIGN;
			++current;

			if (*current == '=') {
				next = _EQUALS_EQ;
				++current;
			}

			if (*current == '>') {
				next = _ARROW;
				++current;
			}
			break;

		case '>':
			next = _GREATER;
			++current;

			if (*current == '=') {
				next = _GREATER_EQ;
				++current;
			}
			break;

		case '<':
			next = _LESSER;
			++current;

			if (*current == '=') {
				next = _LESSER_EQ;
				++current;
			}
			break;

		case '!':
			next = _EXCLAIM;
			++current;

			if (*current == '=')
			{
				next = _NOT_EQ;
				++current;
			}
			break;

			//
			// More Operations
		case '+':
			next = _PLUS;
			++current;

			if (*current == '+') {
				next = _INC;
				++current;
			}

			else if (*current == '='){
				next = _ADD_ASSIGN;
				++current;
			}
			break;

		case '-':
			next = _MINUS;
			++current;

			if (*current == '-') {
				next = _DEC;
				++current;
			}

			else if (*current == '=')
			{
				next = _MINUS_ASSIGN;
				++current;
			}
			break;

			//
			// AND OR
		case '&':
			next = _AMPERSAND;
			++current;

			if (*current == '&') {
				next = _AND;
				++current;
			}
			break;

		case '|':
			next = absolute ? _CLOSE_ABS : _OPEN_ABS;
			++current;

			if (*current == '|') {
				next = _OR;
				++current;
			}
			else {
				absolute = !absolute;
			}
			break;

		case '.':
			++current;
			if (*current == '.') {
				next = _RANGE;
				++current;
			}

			else if (std::isalpha(*current) || *current == '_')
			{
				next = _PROPERTY;
				word = "";
				do {
					word += *current;
					++current;
				} while (std::isalpha(*current) || *current == '_' || std::isdigit(*current));
			}

			else {
				throw parser_error("Unexpected standalone period.");
			}
			break;

			//
			// Strings and characters
		case '\'':
		case '\"':
		{
			// Local trackers
			std::string s;
			char q = *current;

			// Store Token
			next = (q == '\"') ? _STRING : _CHAR;
			++current;

			// Copy string
			while (*current != q)
			{
				s += *current;
				++current;
			}
			++current;

			// Store string
			string = s;

			if (q == '\'') {
				// Store character if it's length 0
				if (string.size() == 1) {
					character = string[0];
				}
				else { // Wrong quotes or character length
					throw parser_error("Characters must have a length of 1.");
				}
			}
		}
		break;

		//
		// Special characters
		case '\\':
		{
			++current;
			next = _CHAR;
			char c = *current;
			++current;
			switch (c)
			{
				// Nulls and special whitespace
			case '0':
				character = L'\0';
				break;
			case 'n':
				character = L'\n';
				break;
			case 'r':
				character = L'\r';
				break;
			case 't':
				character = L'\t';
				break;

				// Hexadecimal
			case 'x':
				character = 0;
				while (std::isxdigit(*current)) {
					character = character * 16 + (*current >= 'a') ? *current - 'a' + 10 : (*current >= 'A') ?
						*current - 'A' + 10 : *current - '0';
					++current;
				}
				break;
			default: // Special character not recognized
				throw parser_error("Could not interpret special character. (Did you forgetÅu\"...\"Åvfor a string?)");
			}
		}
		break;

		//
		// Numeric values
		default:
			if (std::isdigit(*current)) {
				next = _NUMBER;
				number = 0.0;

				do { // Integer part
					number = number * 10. + (*current - '0');
					++current;
				} while (std::isdigit(*current));

				// Decimal part
				if (*current == '.' && std::isdigit(*(current + 1))) {
					++current;

					long double d = 1;
					while (std::isdigit(*current))
					{
						d = d / 10;
						number = number + d * (*current - '0');
						++current;
					}
				}
			}

			// Words
			else if (std::isalpha(*current) || *current == '_')
			{
				next = _WORD;
				word = "";

				// User Identifiers
				do {
					word += *current;
					++current;
				} while (std::isalpha(*current) || *current == '_' || std::isdigit(*current));

				// Reserved keywords
				if (word == "events") {
					next = _EVENTS;
				}
				else if (word == "new") {
					next = _NEW;
				}
				else if (word == "class") {
					next = _CLASS;
				}
				else if (word == "for") {
					next = _FOR;
				}
				else if (word == "break") {
					next = _BREAK;
				}
				else if (word == "on") {
					next = _ON;
				}
				else if (word == "reverse") {
					next = _REVERSE;
				}
				else if (word == "else") {
					next = _ELSE;
				}
				else if (word == "function") {
					next = _FUNCTION;
				}
				else if (word == "if") {
					next = _IF;
				}
				else if (word == "in") {
					next = _IN;
				}
				else if (word == "let" || word == "var") {
					next = _LET;
				}
				else if (word == "local") {
					next = _LOCAL;
				}
				else if (word == "loop") {
					next = _LOOP;
				}
				else if (word == "return") {
					next = _RETURN;
				}
				else if (word == "sub") {
					next = _SUB;
				}
				else if (word == "task") {
					next = _TASK;
				}
				else if (word == "this") {
					next = _THIS;
				}
				else if (word == "while") {
					next = _WHILE;
				}
				else if (word == "yield") {
					next = _YIELD;
				}
				else if (word == "exit") {
					next = _EXIT;
				}
			}
			else { // Not an accepted token
				next = _INVALID;
			}
		}
	}
}
