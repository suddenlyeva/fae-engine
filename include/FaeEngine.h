#pragma once
#include "fcompiler.h"

namespace fae
{
	//
	// Virtual Machine that executes code generated by the fCompiler
	class FaeEngine
	{
	private:
		// Aliasing
		using variables_t = fVector<fValue>; // For variables
		using stack_t	  = fVector<fValue>; // For the execution stack

		//
		// Disallow default and copy construction
		FaeEngine();
		FaeEngine(FaeEngine const & source);

		//
		// Holds everything needed for a stackful, resumable execution context
		struct environment
		{
			environment * prev, * next; // Next and previous call stack
			environment * parent;		// Parent context
			index ref_count;			// Will release memory when no more references exist
			block * sub;				// Executing code blocks
			index start_index;			// Loop-back location
			variables_t variables;		// Holds all variables
			stack_t stack;				// Holds the execution stack
			bool has_result;			// True if the environment returns a value
		};

		//
		// Iteration pointers
		environment * first_using_environment;
		environment * last_using_environment;
		environment * first_garbage_environment;
		environment * last_garbage_environment;

		// Declarations for building and releasing environments
		environment * new_environment(environment * parent, block * b);
		void dispose_environment(environment * object);

		// Vector of environments simulate execution threads
		fVector<environment *> threads;
		index current_thread_index;

		// Machine flags
		bool finished;
		bool stopped;
		bool resuming;

		// Passes execution context to the next thread
		void yield()
		{
			if (current_thread_index > 0)
				--current_thread_index;
			else
				current_thread_index = threads.length - 1;
		}

		void advance();

	public:
		
		//
		// Machine contains the compiler (For now)
		fCompiler * compiler;

		//
		// Error handling
		bool error;
		std::string error_message;
		index error_line;

		//
		// Prototype declarations

		// Constructors
		FaeEngine(fCompiler * compiler);
		virtual ~FaeEngine();

		//
		// Void methods
		void run();
		void call(std::string & event_name);
		void resume();

		//
		// Stops the machine
		void stop()
		{
			finished = true;
			stopped = true;
		}

		//
		// Read flags

		bool is_stopped() const
		{
			return stopped;
		}

		bool is_resuming() const
		{
			return resuming;
		}

		bool has_error() const
		{
			return error;
		}

		std::string & get_error_message()
		{
			return error_message;
		}

		index get_error_line() const
		{
			return error_line;
		}

		void raise_error(std::string const & message)
		{
			error = true;
			error_message = message;
			finished = true;
		}

		bool has_event(std::string const & event_name);

		index get_current_line();

	};
}