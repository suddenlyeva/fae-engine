#pragma once
#include "fvalue.hpp"

namespace fae
{
	// Forward declarations
	struct block;

	//
	// Tokens passed by the parser
	enum instruction
	{
		ASSIGN,
		ASSIGN_WRITABLE, // Maybe not needed
		BREAK_LOOP,
		BREAK_ROUTINE,
		CALL,
		CALL_PUSH,
		CASE_BEGIN,
		CASE_END,
		CASE_IF,
		CASE_IF_NOT,
		CASE_NEXT,
		CMP_EQ,
		CMP_GT,
		CMP_GTEQ,
		CMP_LT,
		CMP_LTEQ,
		CMP_NEQ,
		DUP,
		DUP2,
		LOOP_ASC,
		LOOP_BACK,
		LOOP_COUNT,
		LOOP_DESC,
		LOOP_IF,
		POP,
		PUSH_VAL,
		PUSH_VAR,
		PUSH_VAR_WRITABLE, // Maybe not needed
		SWAP,
		YIELD,
		EXIT_WITH_VALUE
	};

	//
	// A single code instruction for the machine to execute
	struct code
	{
		instruction command; // Instruction to execute
		index		line;    // Line number in the source code
		fValue		data;    // Value to PUSH_VAL onto the execution stack

		//
		// Additional instruction information for some tokens
		union
		{
			// Variable identifiers for ASSIGN and PUSH_VAR
			struct
			{
				index level;	// Indicates depth of environment scope
				index variable;	// Index of the variable inside the scope
			};

			// Necessary information for function calls
			struct
			{
				block * jump_location;	// Block of code to execute in CALL and CALL_PUSH
				index	argument_count;	// Number of function arguments
			};

			// Return destination for LOOP_BACK
			struct
			{
				index start_index;
			};
		};

		//
		// Code Constructors
		//

		//
		// Default
		code()
		{
		}

		//
		// General Constructor
		code(index line, instruction GENERAL)
			: line(line), command(GENERAL)
		{
		}

		//
		// ASSIGN/PUSH_VAR Constructor
		code(index line, instruction VAR, index level, index var_id)
			: line(line), command(VAR), level(level), variable(var_id)
		{
		}

		//
		// CALL/CALL_PUSH Constructor
		code(index line, instruction CALL, block * jump, index argc)
			: line(line), command(CALL), jump_location(jump), argument_count(argc)
		{
		}

		//
		// LOOP_BACK Constructor
		code(index line, instruction LOOP, index ret)
			: line(line), command(LOOP), start_index(ret)
		{
		}

		//
		// PUSH_VAL Constructor
		code(index line, instruction PUSH, fValue data)
			: line(line), command(PUSH), data(data)
		{
		}
	};

}