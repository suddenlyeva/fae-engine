#pragma once
#include <unicode/unistr.h>
#include "fvector.hpp"

namespace fae
{
	class fType;
	using typehead   = fType *;
	using identifier = std::string;

	// Possible primitive types
	enum primitive
	{
		VOID,
		BOOL,
		NUMBER,
		CHAR,
		ARRAY,
		OBJECT
	};

	//
	// Holds a type for comparison
	class fType
	{
	public:

		//
		// Fields
		//

		// Every value has a primitive associated with it.
		const primitive base;

		// For array children
		const typehead inner_type;

		// For typed object polymorphism and function linking
		index head_length;
		fVector<identifier>	poly_names;
		fVector<typehead>	poly_types;

		//
		// Constructors
		//

		//
		// Null Constructor
		fType() : base(primitive::VOID), inner_type(nullptr), head_length(0)
		{
		}

		//
		// Primitive Constructor
		explicit fType(primitive const & p) : base(p), inner_type(nullptr), head_length(0)
		{
		}

		//
		// Array Constructor
		explicit fType(primitive const & ARRAY, typehead const & element)
			: base(ARRAY), inner_type(element), head_length(0)
		{
		}

		//
		// Base Class Type Constructor
		explicit fType(primitive const & OBJECT, identifier const & name)
			: base(OBJECT), head_length(1), inner_type(nullptr)
		{
			// Store a new name
			poly_names = fVector<identifier>();
			poly_names.push(name);

			// Store a new type
			poly_types = fVector<typehead>();
			poly_types.push(this);
		}

		//
		// Inherited Class Type Constructor
		explicit fType(primitive const & OBJECT, identifier const & name, typehead const & parent)
			: base(OBJECT), head_length(1), inner_type(nullptr)
		{
			// Inherit names and add new to front
			poly_names = parent->poly_names;
			poly_names.insert(0, name);

			// Inherit types and add new to front
			poly_types = parent->poly_types;
			poly_types.insert(0, this);
		}
		
		//
		// Union Class Type Constructor
		explicit fType(primitive const & OBJECT, fVector<typehead> const & parents)
			: base(OBJECT), inner_type(nullptr)
		{
			// Store into new vectors
			head_length = 0;
			poly_names = fVector<identifier>();
			poly_types = fVector<typehead>();

			// Cache indices
			index i, j;

			// Loop through each typehead in order
			for (i = 0; i < parents.length; ++i) {
				for (j = 0; j < parents[i]->head_length; ++j) {

					// Add to front of new type
					++head_length;
					poly_names.push(parents[i]->poly_names[j]);
					poly_types.push(parents[i]->poly_types[j]);
				}
			}
			// Loop through each tail in order
			for (i = 0; i < parents.length; ++i) {
				for (j = parents[i]->head_length; j < parents[i]->poly_types.length; ++j) {

					// Add to type
					poly_names.push(parents[i]->poly_names[j]);
					poly_types.push(parents[i]->poly_types[j]);
				}
			}
		}

		//
		// Use the Default Destructor

		//
		// Methods
		//

		//
		// Polymorphic type matching
		const bool has_polytype(typehead const & dest) const
		{
			// Generic object
			if (dest->head_length == 0) {
				return true;
			}
			// Single type matching
			else if (dest->head_length == 1) {
				for (index i = 0, len = poly_types.length; i < len; ++i) {
					if (dest == poly_types[i]) {
						return true;
					}
				}
			}
			else { // Union matching

				index matches = 0;
				// For each union type in head
				for (index i = 0; i < dest->head_length; ++i) {
					// Check all polytypes
					for (index j = 0; j < poly_types.length; ++j) {
						if (dest->poly_types[i] == poly_types[j]) {
							++matches;
							// Return true if every type had a match
							if (matches == dest->head_length) {
								return true;
							}
						}
					}
				}
			}
			// No matches found
			return false;
		}
	};
}

// Every value holds a pointer to a "type" that is registered by the machine.
// Each "different" type only occupies one registry in the machine.
// Therefore, values have the EXACT same type if and only if they point to the same registry in the machine.
// 
// For arrays, there are two comparisons that matter.
// First arrays can typically operate with other arrays, thus storing primitive::ARRAY as a type.
// Next, arrays must have the same contents, thus storing an inner_type to compare with.
//
// For object comparison, the right-hand side object must contain the left hand side as one of its types.
// In other words, the right must be polymorphically larger than the left.
// The provided poly_types vector can be linearly searched for a pointer match.
// Since typed function calls will require an iteration of possible types, we also hold a poly_name inside the type.
