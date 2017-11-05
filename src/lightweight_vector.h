#pragma once
#include <cstdint>

namespace fae 
{
	// Expandable array type
	// Provides quick insertions and deletions
	template <typename T>
	class lightweight_vector
	{
	public:
		// Use fastest integer type for indexing
		using index = std::uint_fast32_t;

		// Fields
		index length;	// Dynamic size
		index capacity;	// Memory buffer
		T * at;			// Starting address

		//
		// Constructors
		//

		//
		// Default Constructor
		lightweight_vector() : length(0), capacity(0), at(nullptr)
		{
		}

		//
		// Copy Constructor
		lightweight_vector(lightweight_vector const & source)
		{
			// Copy fields
			length = source.length;
			capacity = source.capacity;

			// Copy existing elements
			if (capacity > 0) {
				at = new T[capacity];
				for (index i = 0; i < source.length; ++i) {
					at[i] = source.at[i];
				}
			}
			else { // Source is empty
				at = nullptr;
			}
		}

		//
		// Destructor
		~lightweight_vector()
		{
			if (at != nullptr) {
				delete[] at;
			}
		}

		//
		// Operators
		//

		//
		// Copy Assignment
		lightweight_vector & operator = (lightweight_vector const & source)
		{
			// Replace current data
			if (at != nullptr) {
				delete[] at;
			}

			// Copy fields
			length = source.length;
			capacity = source.capacity;

			// Copy existing elements
			if (capacity > 0) {
				at = new T[capacity];
				for (index i = 0; i < source.length; ++i) {
					at[i] = source.at[i];
				}
			}
			else { // Source is empty
				at = nullptr;
			}

			return *this;
		}

		//
		// Read Access
		T const & operator[] (index const & i) const
		{
			return at[i];
		}

		//
		// Write Access
		T & operator[] (index const & i)
		{
			return at[i];
		}

		//
		// Methods
		//

		//
		// Allocates more memory
		void expand()
		{
			// Default capacity of 4
			if (capacity == 0) {
				capacity = 4;
				at = new T[4];
			}
			else { // Contents exist

				// Recreate buffer with double capacity
				capacity *= 2;
				T * temp = new T[capacity];

				// Copy old contents and free old memory
				for (index i = 0; i < length; ++i) {
					temp[i] = at[i];
				}
				// Replace buffer
				delete[] at;
				at = temp;
			}
		}

		//
		// Frees all memory
		void release()
		{
			if (at != nullptr) {
				length = 0;
				capacity = 0;
				delete[] at;
				at = nullptr;
			}
		}

		//
		// Adds an element to the end
		void push_back(T const & value)
		{
			if (length == capacity) {
				expand();
			}
			at[length] = value;
			++length;
		}

		//
		// Removes an element from the end
		void pop_back()
		{
			if (length != 0) {
				--length;
			}
		}

		//
		// Removes all elements
		void clear()
		{
			length = 0;
		}

		//
		// Erases an element at specified index
		void erase(index const & target)
		{
			pop_back();
			// Shift out
			for (index i = target; i < length; ++i)
			{
				at[i] = at[i + 1];
			}
		}

		//
		// Inserts an element at specified index
		void insert(index const & dest, T const & value)
		{
			// Expand if necessary
			if (length == capacity) {
				expand();
			}
			// Shift over
			for (index i = length; i > dest; --i)
			{
				at[i] = at[i - 1];
			}
			// Copy in
			at[dest] = value;
			++length;
		}
	};
}
