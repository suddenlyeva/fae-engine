#pragma once
#include "fvector.h"
#include "fvalue.h"
#include "fcode.h"

namespace fae
{
	// Forward declarations
	class FaeEngine;

	//
	// Points to a function that returns an fValue
	using callback = fValue (*)(FaeEngine * machine, index const & argc, fValue const * argv);

	//
	// Function handle for the execution layer
	struct function
	{
		identifier name;
		callback func;
		index argument_count;
	};

	//
	// Holds an array of code tokens to execute, and other necessary information to pass in
	struct block
	{
		// Types of Code Blocks
		enum type
		{
			NORMAL, LOOP, SUB, FUNCTION, TASK
		};

		index level; // Depth of environment scope
		type  kind;	 // Type of the block

		identifier name;		// Identifier used to call
		index argument_count;	// Argument count for FUNCTION and TASK
		typehead caller_type;	// Type compatibility for this.call() notation
		callback function;		// For function callback blocks
		fVector<code> codes;	// For explicit bytecode

		//
		// Constructor for blocks
		block(index const & level, type const & type) 
			: level(level), kind(type), argument_count(0), name(), 
			  function(nullptr), codes(), caller_type(nullptr)
		{
		}
	};
}
