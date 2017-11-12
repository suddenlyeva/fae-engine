#pragma once
#include <list>
#include <unordered_map>
#include "fType.hpp"

namespace fae 
{
	class fTypeManager
	{
	private:
		// The types need to be in a container where the address does not change.
		std::list<fType> types;

		//
		// For convenience only
		std::unordered_map<std::string, typehead> primitives;
		void register_primitive(std::string const & id, fType const & type)
		{
			primitives[id] = &* types.insert(types.end(), type);
		}

	public:
		//
		// Constructors
		//

		//
		// Initial Constructor Registers all primitive types
		fTypeManager()
		{
			register_primitive("NULL",	 fType());
			register_primitive("[VOID]", fType(primitive::ARRAY));
			register_primitive("OBJECT", fType(primitive::OBJECT));
			register_primitive("BOOL",	 fType(primitive::BOOL));
			register_primitive("NUMBER", fType(primitive::NUMBER));
			register_primitive("CHAR",	 fType(primitive::CHAR));
			register_primitive("STRING", fType(primitive::ARRAY, primitives["CHAR"]));
		}

		//
		// Methods
		//

		//
		// Register a new dynamic type
		const typehead add(fType const & object)
		{
			return &* types.insert(types.end(), object);
		}

		//
		// Get the type associated with an array of specific types
		const typehead arrayof(typehead element)
		{
			for (auto & it : types) {
				if (it.base == primitive::ARRAY && it.inner_type == element) {
					return & it;
				}
			}
			return &* types.insert(types.end(), fType(primitive::ARRAY, element));
		}

		//
		// Get the type associated with a union of specific types
		const typehead unionof(fType const & polyunion)
		{
			for (auto & it : types) {

				// Same head size
				if (it.head_length == polyunion.head_length) {

					// Everything in the head matches
					for (index i = 0; it.poly_types[i] == polyunion.poly_types[i]; ++i) {
						if (i == it.head_length - 1) {
							return & it;
						}
					}
				}
			}
			// Else it's a new union
			return &* types.insert(types.end(), polyunion);
		}

		//
		// Get a primitive type
		const typehead primitive(std::string const & id) const
		{
			return primitives.at(id);
		}
	};

}
