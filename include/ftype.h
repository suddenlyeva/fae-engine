#pragma once
#include <unicode/unistr.h>
#include "fvector.h"

namespace fae
{
	class fType;
	using typehead   = fType const *;
	using identifier = std::string const;

	// Possible primitive types
	enum primitive
	{
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
		const index head_length;
		fVector<identifier>	poly_names;
		fVector<typehead>	poly_types;

		//
		// Constructors
		//

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
		// Base Object Type Constructor
		explicit fType(primitive const & OBJECT, identifier & name)
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
		// Inherited Object Type Constructor
		explicit fType(primitive const & OBJECT, identifier & name, typehead const & parent)
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
		// Union Object Type Constructor
		explicit fType(primitive const & OBJECT, fVector<typehead> const & parents)
			: base(OBJECT), head_length(parents.length), inner_type(nullptr)
		{
			// Store into new vectors
			poly_names = fVector<identifier>();
			poly_types = fVector<typehead>();

			// Cache indices
			index i, j;

			// Loop through each typehead in order
			for (i = 0; i < head_length; ++i) {
				for (j = 0; j < parents[i]->head_length; ++j) {

					// Add to front of new type
					poly_names.push(parents[i]->poly_names[j]);
					poly_types.push(parents[i]->poly_types[j]);
				}
			}
			// Loop through each tail in order
			for (i = 0; i < head_length; ++i) {
				for (j = parents[i]->head_length; j < parents.length; ++j) {

					// Add to type
					poly_names.push(parents[i]->poly_names[j]);
					poly_types.push(parents[i]->poly_types[j]);
				}
			}
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
