#pragma once
#include <list>
#include <unordered_map>
#include "fType.h"

namespace fae 
{
	class fTypeManager
	{
	private:
		// The types need to be in a container where the address does not change.
		std::list<const fType> types;

		//
		// For convenience only
		std::unordered_map<identifier, typehead> primitives;
		void register_primitive(identifier & id, fType const & type)
		{
			primitives[id] = *& types.insert(types.end(), type);
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
			register_primitive("[NULL]", fType(primitive::ARRAY));
			register_primitive("{NULL}", fType(primitive::OBJECT));
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
			return *& types.insert(types.end(), object);
		}

		//
		// Get the type associated with an array of specific types
		const typehead array(typehead const & element)
		{
			for (auto & it : types) {
				if (it.base == primitive::ARRAY && it.inner_type == element) {
					return & it;
				}
			}
			return *& types.insert(types.end(), fType(primitive::ARRAY, element));
		}

		//
		// Get a primitive type
		const typehead primitive(identifier & id) const
		{
			return primitives.at(id);
		}
	};

}
