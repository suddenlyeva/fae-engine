#pragma once
#include "FaeEngine.hpp"

namespace fae
{
	//
	// Operations
	//

	//
	// Addition
	fValue add(FaeEngine * machine, index argc, fValue * argv)
	{
		return fValue(machine->typeof("NUMBER"), argv[0].number() + argv[1].number());
	}

	//
	// Subtraction
	fValue subtract(FaeEngine * machine, index argc, fValue * argv)
	{
		return fValue(machine->typeof("NUMBER"), argv[0].number() - argv[1].number());
	}

	//
	// Multiplication
	fValue multiply(FaeEngine * machine, index argc, fValue * argv)
	{
		return fValue(machine->typeof("NUMBER"), argv[0].number() * argv[1].number());
	}

	//
	// Division
	fValue divide(FaeEngine * machine, index argc, fValue * argv)
	{
		return fValue(machine->typeof("NUMBER"), argv[0].number() / argv[1].number());
	}

	//
	// Remainder
	fValue mod(FaeEngine * machine, index argc, fValue * argv)
	{
		return fValue(machine->typeof("NUMBER"), std::fmodl(argv[0].number(), argv[1].number()));
	}

	//
	// Negation
	fValue negate(FaeEngine * machine, index argc, fValue * argv)
	{
		return fValue(machine->typeof("NUMBER"), -argv[0].number());
	}

	//
	// Power
	fValue power(FaeEngine * machine, index argc, fValue * argv)
	{
		return fValue(machine->typeof("NUMBER"), std::powl(argv[0].number(), argv[1].number()));
	}

	//
	// Comparison
	fValue compare(FaeEngine * machine, index argc, fValue * argv)
	{
		// Base type must be the same to compare
		if (argv[0].type() == argv[1].type()) {

			// Store the result
			long double result;

			// Case on primitive types
			switch (argv[0].type()->base)
			{
				// Boolean Type
			case primitive::BOOL: {

				bool a = argv[0].boolean();
				bool b = argv[1].boolean();
				result = (a == b) ? 0 : (a < b) ? -1 : +1;
			} break;

				// Number Type
			case primitive::NUMBER: {

				long double a = argv[0].number();
				long double b = argv[1].number();
				result = (a == b) ? 0 : (a < b) ? -1 : +1;
			} break;

				// Character Type
			case primitive::CHAR: {

				UChar a = argv[0].character();
				UChar b = argv[1].character();
				result = (a == b) ? 0 : (a < b) ? -1 : +1;
			} break;

				// Array Type
			case primitive::ARRAY: {

				// Loop through both arrays
				for (index i = 0, len = argv[0].array_length(); i < len; ++i) {

					// The first array is longer
					if (i >= argv[1].array_length()) {
						result = +1;
						break;
					}

					// For Strings, compare alphabetically
					if (argv[0].type() == machine->typeof("STRING")) {
						fValue v[2];
						v[0] = argv[0].read_index(i);
						v[1] = argv[1].read_index(i);
						result = compare(machine, 2, v).number();
					}

					// A String with greater value was found.
					if (result != 0) {
						break;
					}
				}
				// The second array is longer
				if (result == 0 && argv[0].array_length() < argv[1].array_length()) {
					result = -1;
				}
			} break;

			case primitive::OBJECT: {

				// Initialize to Null
				fValue v[2] = { fValue(), fValue() };

				// Store explicit value, recursively
				while (argv[0].has_property("value")) {
					v[0] = argv[0].get_property("value");
				}
				while (argv[1].has_property("value")) {
					v[1] = argv[1].get_property("value");
				}

				// If both are not null, we can attempt a comparison
				if (!v[0].is_null() && !v[1].is_null()) {
					result = compare(machine, 2, v).number();
				}
				else { // No value was found for one of the objects
					machine->raise_error("Cannot compare objects without a \"value\" property.");
					return fValue();
				}
			} break;

			default: // Null == Null
				machine->raise_error("A null type comparison was made.");
				return fValue();
			}
			// Return the result, -1 or 0 or +1
			return fValue(machine->typeof("NUMBER"), result);
		}
		else { // The types are different, return null
			machine->raise_error("Cannot compare values of different types.");
			return fValue();
		}
	}

	//
	// Decrement
	fValue decrement(FaeEngine * machine, index argc, fValue * argv)
	{
		// Case on type
		switch (argv[0].type()->base)
		{
			// Number Type
		case primitive::NUMBER:
			return fValue(argv[0].type(), argv[0].number() - 1);

			// Character Type
		case primitive::CHAR: {
			UChar c = argv[0].character();
			--c;
			return fValue(argv[0].type(), c);
		}
			// Object Type
		case primitive::OBJECT: {

			// Initialize Null
			fValue v = fValue();

			// Store explicit value, recursively
			while (argv[0].has_property("value")) {
				v = argv[0].get_property("value");
			}
			// Value was found
			if (!v.is_null()) {
				return fValue(v.type(), v.number() - 1);
			}
			else { // No value was found
				machine->raise_error("Cannot decrement object without a \"value\" property.");
				return fValue();
			}
		}
		default:
			machine->raise_error("Decrement operations cannot be used with this type.");
			return fValue();
		}
	}

	//
	// Increment
	fValue increment(FaeEngine * machine, index argc, fValue * argv)
	{
		// Case on type
		switch (argv[0].type()->base)
		{
			// Number Type
		case primitive::NUMBER:
			return fValue(argv[0].type(), argv[0].number() + 1);

			// Character Type
		case primitive::CHAR: {
			UChar c = argv[0].character();
			++c;
			return fValue(argv[0].type(), c);
		}
			// Object Type
		case primitive::OBJECT: {

			// Initialize Null
			fValue v = fValue();

			// Store explicit value, recursively
			while (argv[0].has_property("value")) {
				v = argv[0].get_property("value");
			}
			// Value was found
			if (!v.is_null()) {
				return fValue(v.type(), v.number() + 1);
			}
			else { // No value was found
				machine->raise_error("Cannot decrement object without a \"value\" property.");
				return fValue();
			}
		}
		default:
			machine->raise_error("Decrement operations cannot be used with this type.");
			return fValue();
		}
	}

	//
	// True
	fValue true_(FaeEngine * machine, index argc, fValue * argv)
	{
		return fValue(machine->typeof("BOOL"), true);
	}

	//
	// False
	fValue false_(FaeEngine * machine, index argc, fValue * argv)
	{
		return fValue(machine->typeof("BOOL"), false);
	}

	//
	// Not
	fValue not_(FaeEngine * machine, index argc, fValue * argv)
	{
		return fValue(machine->typeof("BOOL"), !argv[0].boolean());
	}

	//
	// Read index
	fValue read_index(FaeEngine * machine, index argc, fValue * argv)
	{
		// Check for Array
		if (argv[0].type()->base != primitive::ARRAY) {
			machine->raise_error("Attempted to index a non-array value.");
			return fValue();
		}

		// Check negative index
		if (argv[1].number() < 0) {
			machine->raise_error("Array index is negative.");
			return fValue();
		}

		// Store index
		index i = static_cast<index> (argv[1].number() + 0.5);

		// Check out of bounds
		if (i >= argv[0].array_length()) {
			machine->raise_error("Array index is out of bounds.");
			return fValue();
		}

		// All clear
		return argv[0].read_index(i);
	}

	//
	// Write index
	fValue write_index(FaeEngine * machine, index const & argc, fValue * const & argv)
	{
		// Check for Array
		if (argv[0].type()->base != primitive::ARRAY) {
			machine->raise_error("Attempted to index a non-array value.");
			return fValue();
		}

		// Check negative index
		if (argv[1].number() < 0) {
			machine->raise_error("Array index is negative.");
			return fValue();
		}

		// Store index
		index i = static_cast<index> (argv[1].number() + 0.5);

		// Check out of bounds
		if (i >= argv[0].array_length()) {
			machine->raise_error("Array index is out of bounds.");
			return fValue();
		}

		// Check type
		if (argv[0].type()->inner_type != argv[2].type()) {

			// Only allow object to object polymorphism
			if (argv[0].type()->inner_type->base != primitive::OBJECT ||
				argv[2].type()->base != primitive::OBJECT) {
				machine->raise_error("Type mismatch on array assignment.");
				return fValue();
			}

			// Check polymorphic type match
			if (argv[0].type()->inner_type != machine->typeof("[VOID]") &&
			   !argv[2].type()->has_polytype(argv[0].type())) {
				machine->raise_error("Object type mismatch on array assignment.");
				return fValue();
			}
		}

		// All clear
		argv[0].write_index(i, argv[2]);
	}
}
