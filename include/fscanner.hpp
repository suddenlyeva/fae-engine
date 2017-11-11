#pragma once
#include "FaeEngine.hpp"

namespace fae
{
	//
	// Parser Error class
	class parser_error : public std::exception 
	{
	public:

		//
		// Constructor
		parser_error(std::string const & the_message) : message(the_message)
		{
		}

		//
		// Exception Message
		virtual const char * what() const throw()
		{
			return message.c_str();
		}

	private:
		std::string message;
	};

	// Scanner tokens that are passed to parser
	enum token
	{
		// Error types
		_END, _INVALID, 
		// Words and .words
		_WORD, _PROPERTY, 
		// Basic types
		_NUMBER, _CHAR, _STRING,
		// Bracket types
		_OPEN_PAR, _CLOSE_PAR, _OPEN_BRA, _CLOSE_BRA, _OPEN_CUR, _CLOSE_CUR, _ABS,
		// Punctuation types
		_AMPERSAND, _ARROW, _AT, _COMMA, _COLON, _RANGE, _SEMICOLON,
		// Assignment types
		_ASSIGN, _ADD_ASSIGN, _MINUS_ASSIGN, _MULT_ASSIGN, _DIV_ASSIGN, 
		_MOD_ASSIGN, _POW_ASSIGN, _LINK_ASSIGN,
		// Operation types
		_TILDE, _PLUS, _MINUS, _INC, _DEC, _ASTERISK, _SLASH, _PERCENT, _CARET,
		// Logic types
		_EQUALS_EQ, _GREATER, _GREATER_EQ, _LESSER, _LESSER_EQ, _NOT_EQ, _EXCLAIM, _AND, _OR, 
		// Reserved Words
		_BREAK, _CLASS, _ELSE, _EXIT, _FOR, _EVENTS, _FUNCTION, _IF, _IN, _LET,
		_LOCAL, _LOOP, _NEW, _ON, _RETURN, _REVERSE, _SUB, _TASK, _THIS, _WHILE, _YIELD
	};

	class fScanner
	{
	public:
		//
		// Tracking Fields
		char const * current; // Current character
		token next;			  // Token passed to parser
		std::string word;	  // Currently stored word
		long double number;	  // Currently stored number
		UChar character;	  // Currently stored character
		std::string string;	  // Currently stored string value
		index line;			  // Current line in the source code

		//
		// Constructor
		fScanner(char const * source) : current(source), line(1)
		{
			advance();
		}

		//
		// Copy Constructor
		fScanner(fScanner const & source)
			: current(source.current), next(source.next),
			  word(source.word), line(source.line)
		{
		}

		//
		// Skip whitespace and comments
		void skip();

		// Scan next token
		void advance();
	};
}
