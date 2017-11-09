#pragma once
#include "fvalue.hpp"
#include "fvector.hpp"
#include "fblock.hpp"
#include "ftypemanager.hpp"
#include <map>

namespace fae
{
	//
	// Compiles a script into intermediate bytecode
	class fCompiler
	{
	public:
		//
		// Fields
		//

		//
		// For Error Handling
		bool error;
		std::string error_message;
		index error_line;

		//
		// Block containers
		std::list<block> blocks;	// Use double linked list for constant addresses
		block * main_block;			// Pointer to global scope
		std::map<identifier, block *> events;  // Machine events to be called by execution layer

		//
		// For type compatibility
		fTypeManager * types;

		//
		// Constructors
		//

		//
		// Main Constructor
		fCompiler(fTypeManager * type_manager, std::string const & script, index const & funcc, function const * funcv);

		//
		// Destructor
		~fCompiler()
		{
			blocks.clear();
		}

		//
		// Methods
		//

		//
		// Adds a block to the machine and returns the address
		block * new_block(index const & level, block::type const & type)
		{
			// Dereferencing blocks.insert will return the address of the inserted block
			return &* blocks.insert(blocks.end(), (block(level, type)));
		}

		// void * data // Space for the client? Probably not needed
	};
}