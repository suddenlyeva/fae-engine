#pragma once
#include <unordered_map>
#include <variant>
#include <unicode/ustdio.h>
#include "ftype.h"

namespace fae
{
	class fValue;
	using fObject = std::unordered_map<identifier, fValue>;
	using fData   = std::variant<bool, long double, UChar, fVector<fValue>, fObject *>;

	//
	// Generic dynamically typed data holder
	class fValue
	{
	private:

		//
		// Data content structure
		struct body
		{
			index ref_count;
			typehead type;
			fData contents;
		};

		body * data;

		//
		// For accessing contents

		// Array
		fVector<fValue> & array() const
		{
			return std::get<fVector<fValue>>(data->contents);
		}

		// Object
		fObject *& object() const
		{
			return std::get<fObject>(data->contents);
		}

		// 
		// Garbage Cleanup
		void cleanup()
		{
			if (data != nullptr) {
				--(data->ref_count);
				if (data->ref_count == 0) {
					if (data->type->base == primitive::OBJECT) {
						delete object();
					}
					delete data;
				}
			}
		}

		//
		// Recreate as Clone
		void unique()
		{
			if (data == nullptr)
			{
				data = new body;
				data->ref_count = 1;
				data->type = nullptr;
			}
			else if (data->ref_count > 1)
			{
				--(data->ref_count);
				data = new body(*data);
				data->ref_count = 1;
				if (data->type->base == primitive::OBJECT) {
					data->contents = new fObject(*object());
				}
			}
		}
		
	public:
		//
		// Constructors
		//

		//
		// Null Constructor
		fValue() : data(nullptr)
		{
		}

		//
		// Boolean Constructor
		explicit fValue(typehead const & type, bool const & value)
		{
			data = new body;
			data->ref_count = 1;
			data->type = type;
			data->contents = value;
		}

		//
		// Number Constructor
		explicit fValue(typehead const & type, long double const & value)
		{
			data = new body;
			data->ref_count = 1;
			data->type = type;
			data->contents = value;
		}

		//
		// Character Constructor
		explicit fValue(typehead const & type, UChar const & value)
		{
			data = new body;
			data->ref_count = 1;
			data->type = type;
			data->contents = value;
		}

		//
		// Existing Array Constructor
		explicit fValue(typehead const & type, fVector<fValue> const & value)
		{
			data = new body;
			data->ref_count = 1;
			data->type = type;
			data->contents = value;
		}

		//
		// Existing Object Constructor
		explicit fValue(typehead const & type, fObject * const & value)
		{
			data = new body;
			data->ref_count = 1;
			data->type = type;
			data->contents = value;
		}

		//
		// Empty Object or Array Constructor
		explicit fValue(typehead const & type)
		{
			data = new body;
			data->ref_count = 1;
			data->type = type;
			if (type->base == primitive::OBJECT) {
				data->contents = new fObject();
			}
			if (type->base == primitive::ARRAY) {
				data->contents = fVector<fValue>();
			}
		}

		//
		// String Constructor
		explicit fValue(typehead const & string_t, std::string const & string)
		{
			data = new body;
			data->ref_count = 1;
			data->type = string_t;
			data->contents = fVector<fValue>();

			icu::UnicodeString ustring = icu::UnicodeString::fromUTF8(string);
			for (index i = 0; i < ustring.length(); ++i) {
				array().push(fValue(string_t->inner_type, ustring[i]));
			}
		}

		//
		// Copy Constructor
		fValue(fValue const & source)
		{
			data = source.data;
			if (data != nullptr) {
				++(data->ref_count);
			}
		}

		// Destructor
		~fValue()
		{
			cleanup();
		}

		//
		// Operators
		//

		//
		// Copy Assignment
		fValue & operator = (fValue const & source)
		{
			// Add reference if source exists
			if (source.data != nullptr) {
				++(source.data->ref_count);
			}
			cleanup();

			data = source.data;
			return *this;
		}


		//
		// Methods
		//

		//
		// Get the Type
		typehead type() const
		{
			return data->type;
		}

		//
		// Object Related

		// Register a property
		void register_property(identifier & name, fValue const & property)
		{
			unique();
			object()->try_emplace(name, property);
		}

		// Set a property
		void set_property(identifier & name, fValue const & property)
		{
			object()->at(name) = property;
		}
	};
}