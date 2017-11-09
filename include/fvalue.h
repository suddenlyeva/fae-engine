#pragma once
#include <unordered_map>
#include <variant>
#include <unicode/numfmt.h>
#include "ftype.h"

namespace fae
{
	class fValue;
	using fObject = std::unordered_map<identifier, fValue>;
	using fData   = std::variant<bool, long double, UChar, fVector<fValue>, fObject>;

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

		//
		// Hold the data as a pointer
		body * data;

		//
		// For accessing contents locally

		// Array
		fVector<fValue> & array() const
		{
			return std::get<fVector<fValue>>(data->contents);
		}

		// Object
		fObject & object() const
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
					delete data;
				}
			}
		}

		//
		// Create a separate instance of data
		void unique()
		{
			// New null type instance
			if (data == nullptr)
			{
				data = new body;
				data->ref_count = 1;
				data->type = nullptr;
			}
			// New data instance from multiple references to existing data
			else if (data->ref_count > 1)
			{
				--(data->ref_count);
				data = new body(*data);
				data->ref_count = 1;
			}
			// If a value only has a single reference, it is already unique
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
		explicit fValue(typehead const & type, fObject const & value)
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
				data->contents = fObject();
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

			// Convert to Unicode multi-byte character set
			icu::UnicodeString ustring = icu::UnicodeString::fromUTF8(string);

			// Store into array as UTF-16 code points
			for (index i = 0; i < ustring.length(); ++i) {
				array().push(fValue(string_t->inner_type, ustring[i]));
			}
		}

		//
		// Copy Constructor
		fValue(fValue const & source)
		{
			// Just add a reference, don't clone needed
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
			// Cleanup old data
			cleanup();

			// Copy in and return
			data = source.data;
			return *this;
		}

		//
		// Methods
		//

		//
		// Get the Type
		typehead & type() const
		{
			return data->type;
		}

		//
		// Object Related

		// Note that objects do not call unique, to pass by reference

		// Get a property
		const fValue get_property(identifier & name) const
		{
			return object().at(name);
		}

		// Register a property
		void register_property(identifier & name, fValue const & property)
		{
			object().try_emplace(name, property);
		}

		// Set a property
		void set_property(identifier & name, fValue const & property)
		{
			object().at(name) = property;
		}

		// Clone another object
		void clone_object(fValue const & source)
		{
			object() = source.object();
		}

		// Object Union
		void union_object(fValue const & source)
		{
			for (auto & it : source.object()) {
				object().insert_or_assign(it.first, it.second);
			}
		}

		//
		// Array related

		// Read an index
		const fValue read_index(index const & i) const
		{
			return array().at[i];
		}

		// Write to an index
		void write_index(index const & i, fValue const & element)
		{
			unique();
			array().at[i] = element;
		}

		// Append another element
		void append(fValue const & element)
		{
			unique();
			array().push(element);
		}

		// Concatenate another array
		void concatenate(fValue const & tail)
		{
			unique();
			array().concatenate(tail.array());
		}

		//
		// Access contents from outside

		// Boolean
		const bool boolean() const
		{
			return std::get<bool>(data->contents);
		}

		// Number
		const long double number() const
		{
			return std::get<long double>(data->contents);
		}

		// Character
		const UChar character() const
		{
			return std::get<UChar>(data->contents);
		}

		// Convert to String
		const icu::UnicodeString to_unicode() const
		{
			icu::UnicodeString result;

			switch (data->type->base)
			{
				// From Boolean
			case primitive::BOOL :
				return boolean() ? u"true" : u"false";

				// From Number
			case primitive::NUMBER :

				// Format into Unicode
				static UErrorCode success = U_ZERO_ERROR;
				static icu::NumberFormat * nf = icu::NumberFormat::createInstance(success);

				// Check for integer
				long double is_int;
				if (std::modf(number(), &is_int) == 0.0L) {
					return nf->format(static_cast<long long>(number()), result);
				}
				else { // Is a decimal
					return nf->format(static_cast<double>(number()), result);
				}

				// From a character
			case primitive::CHAR :
				result += character();
				return result;

				// From an array
			case primitive::ARRAY :

				// Array is a string
				if (data->type->inner_type->base == primitive::CHAR) {

					// Generate Unicode
					for (index i = 0, len = array().length; i < len; ++i) {
						result += read_index(i).character();
					}
					return result;
				}
				else { // Array contains something else
				    result = u"[";

					// Fill with contents
					for (index i = 0, len = array().length; i < len; ++i) {
						if (i > 0) {
							result += u",";
						}
						result += read_index(i).to_unicode();
					}
					result += u"]";
					return result;
				}
			case primitive::OBJECT :

				// Convert explicit value
				if (object().count("value")) {
					return get_property("value").to_unicode();
				}
				else {
					// TODO
					return u"Object";
				}

			default:
				return u"(INTERNAL-ERROR)";
			}
		}

		// Convert to UTF8
		const std::string to_string() const
		{
			std::string result;
			to_unicode().toUTF8String(result);
			return result;
		}
	};
}
