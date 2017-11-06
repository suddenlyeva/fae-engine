#pragma once
#include <unicode/unistr.h>

namespace fae
{
	// Holds a type for comparison
	class type_data
	{
		enum
		{
			BOOL,
			NUMBER,
			CHAR,
			ARRAY,
			OBJECT
		};

	};
}