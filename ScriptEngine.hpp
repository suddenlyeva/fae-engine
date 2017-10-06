
#if !defined(__SCRIPT_H__)
#define __SCRIPT_H__

#pragma warning (disable:4786)	//STL Warning抑止
#pragma warning (disable:4018)	//signed と unsigned の数値を比較
#pragma warning (disable:4244)	//double' から 'float' に変換
#pragma warning (disable:4996)	//deprecated functions
#include<list>
#include<string>
#include<map>
#include<unordered_map>

//重複宣言チェックをしない
//#define __SCRIPT_H__NO_CHECK_DUPLICATED

//-------- 汎用
namespace gstd
{
	std::string to_mbcs(std::wstring const & s);
	std::wstring to_wide(std::string const & s);

	template < typename T >
	class lightweight_vector
	{
	public:
		unsigned length;
		unsigned capacity;
		T * at;

		lightweight_vector() : length(0), capacity(0), at(NULL)
		{
		}

		lightweight_vector(lightweight_vector const & source);

		~lightweight_vector()
		{
			if (at != NULL)
				delete[] at;
		}

		lightweight_vector & operator = (lightweight_vector const & source);

		void expand();

		void push_back(T const & value)
		{
			if (length == capacity) expand();
			at[length] = value;
			++length;
		}

		void pop_back()
		{
			--length;
		}

		void clear()
		{
			length = 0;
		}

		void release()
		{
			length = 0;
			if (at != NULL)
			{
				delete[] at;
				at = NULL;
				capacity = 0;
			}
		}

		unsigned size() const
		{
			return length;
		}

		T & operator[] (unsigned i)
		{
			return at[i];
		}

		T const & operator[] (unsigned i) const
		{
			return at[i];
		}

		T & back()
		{
			return at[length - 1];
		}

		T * begin()
		{
			return &at[0];
		}

		void erase(T * pos);
		void insert(T * pos, T const & value);
	};

	template < typename T >
	lightweight_vector < T >::lightweight_vector(lightweight_vector const & source)
	{
		length = source.length;
		capacity = source.capacity;
		if (source.capacity > 0)
		{
			at = new T[source.capacity];
			for (int i = length - 1; i >= 0; --i)
				at[i] = source.at[i];
		}
		else
		{
			at = NULL;
		}
	}

	template < typename T >
	lightweight_vector < T > & lightweight_vector < T >::operator = (lightweight_vector < T > const & source)
	{
		if (at != NULL) delete[] at;
		length = source.length;
		capacity = source.capacity;
		if (source.capacity > 0)
		{
			at = new T[source.capacity];
			for (int i = length - 1; i >= 0; --i)
				at[i] = source.at[i];
		}
		else
		{
			at = NULL;
		}
		return *this;
	}

	template < typename T >
	void lightweight_vector < T >::expand()
	{
		if (capacity == 0)
		{
			//delete[] at;
			capacity = 4;
			at = new T[4];
		}
		else
		{
			capacity *= 2;
			T * n = new T[capacity];
			for (int i = length - 1; i >= 0; --i)
				n[i] = at[i];
			delete[] at;
			at = n;
		}
	}

	template < typename T >
	void lightweight_vector < T >::erase(T * pos)
	{
		--length;
		for (T * i = pos; i < at + length; ++i)
		{
			*i = *(i + 1);
		}
	}

	template < typename T >
	void lightweight_vector < T >::insert(T * pos, T const & value)
	{
		if (length == capacity)
		{
			unsigned pos_index = pos - at;
			expand();
			pos = at + pos_index;
		}
		for (T * i = at + length; i > pos; --i)
		{
			*i = *(i - 1);
		}
		*pos = value;
		++length;
	}

	//-------- from here// ここから

	class type_data
	{
	public:
		enum type_kind
		{
			tk_real, tk_char, tk_boolean, tk_array, tk_object
		};

		type_data(type_kind k, type_data * t = NULL) : kind(k), element(t)
		{
		}

		type_data(type_data const & source) : kind(source.kind), element(source.element)
		{
		}

		//let the default constructor //デストラクタはデフォルトに任せる

		type_kind get_kind()
		{
			return kind;
		}

		type_data * get_element()
		{
			return element;
		}

	private:
		type_kind kind;
		type_data * element;
	};

	class value
	{
	private:

		struct object {
			int ref_count;
			std::unordered_map<std::wstring, value> properties;
		};

		struct body
		{
			int ref_count;
			type_data * type;
			lightweight_vector<value> array_value;
			object * object_value = NULL;

			union
			{
				long double real_value;
				wchar_t char_value;
				bool boolean_value;
			};
		};

		mutable	body * data;


	public:
		value() : data(NULL)
		{
		}

		value(type_data * t)
		{
			if (t->get_kind() == t->tk_object) {
				data = new body();
				data->ref_count = 1;
				data->type = t;
				data->object_value = new object();
				data->object_value->ref_count = 1;
			}
			else {
				data = NULL;
			}
		}

		value(type_data * t, long double v)
		{
			data = new body;
			data->ref_count = 1;
			data->type = t;
			data->real_value = v;
		}

		value(type_data * t, wchar_t v)
		{
			data = new body;
			data->ref_count = 1;
			data->type = t;
			data->char_value = v;
		}

		value(type_data * t, bool v)
		{
			data = new body;
			data->ref_count = 1;
			data->type = t;
			data->boolean_value = v;
		}

		value(type_data * t, std::wstring v)
		{
			data = new body();
			data->ref_count = 1;
			data->type = t;
			for (unsigned i = 0; i < v.size(); ++i)
				data->array_value.push_back(value(t->get_element(), v[i]));
		}

		value(type_data * t, object * o)
		{
				data = new body();
				data->ref_count = 1;
				data->type = t;
				data->object_value = o;
				data->object_value->ref_count++;
		}

		value(value const & source)
		{
			data = source.data;
			if (data != NULL) {
				++(data->ref_count);
				if (data->object_value != NULL)
					++(data->object_value->ref_count);
			}
		}

		~value()
		{
			if (data != NULL)
			{
				if (data->object_value != NULL) {
					--(data->object_value->ref_count);
					if (data->object_value->ref_count == 0)
						delete data->object_value;
				}
				--(data->ref_count);
				if (data->ref_count == 0)
					delete data;
			}
		}

		value & operator = (value const & source)
		{
			if (source.data != NULL)
			{
				++(source.data->ref_count);
				if (source.data->object_value != NULL)
					++(source.data->object_value->ref_count);

			}
			if (data != NULL)
			{
				if (data->object_value != NULL) {
					--(data->object_value->ref_count);
					if (data->object_value->ref_count == 0)
						delete data->object_value;
				}
				--(data->ref_count);
				if (data->ref_count == 0)
					delete data;
			}
			data = source.data;
			return *this;
		}

		bool has_data() const
		{
			return data != NULL;
		}

		void set(type_data * t, long double v)
		{
			unique();
			data->type = t;
			data->real_value = v;
		}

		void set(type_data * t, bool v)
		{
			unique();
			data->type = t;
			data->boolean_value = v;
		}

		bool register_property(const std::wstring & name, const value & val)
		{
			if (data->object_value != NULL)
				return std::get<1>(data->object_value->properties.try_emplace(name, val));

			return false;
		}

		const value get_property(const std::wstring & name) const
		{
			if (data->object_value != NULL) {
				if (data->object_value->properties.count(name) != 0) {
					return data->object_value->properties[name];
				}
			}

			return value();
		}

		bool set_property(const std::wstring & name, const value & val)
		{
			if (data->object_value != NULL) {
				data->object_value->properties.at(name) = val;
				return true;
			}

			return false;
		}

		void append(type_data * t, value const & x)
		{
			unique();
			data->type = t;
			data->array_value.push_back(x);
		}

		void concatenate(value const & x)
		{
			unique();
			unsigned l = data->array_value.length;
			unsigned r = x.data->array_value.length;
			unsigned t = l + r;
			if (l == 0)
				data->type = x.data->type;
			while (data->array_value.capacity < t)
				data->array_value.expand();
			for (unsigned i = 0; i < r; ++i)
				data->array_value[l + i] = x.data->array_value.at[i];
			data->array_value.length = t;
		}

		//the following return, not assign from struct: value
		long double as_real() const
		{
			if (data == NULL)
				return 0.0L;
			else
			{
				switch (data->type->get_kind())
				{
				case type_data::tk_real:
					return data->real_value;
				case type_data::tk_char:
					return static_cast < long double > (data->char_value);
				case type_data::tk_boolean:
					return (data->boolean_value) ? 1.0L : 0.0L;
				case type_data::tk_array:
					if (data->type->get_element()->get_kind() == type_data::tk_char)
						return std::atof(to_mbcs(as_string()).c_str());
					else
						return 0.0L;
				default:
					return 0.0L;
				}
			}
		}
		wchar_t as_char() const
		{
			if (data == NULL)
				return 0.0L;
			else
			{
				switch (data->type->get_kind())
				{
				case type_data::tk_real:
					return data->real_value;
				case type_data::tk_char:
					return data->char_value;
				case type_data::tk_boolean:
					return (data->boolean_value) ? L'1' : L'0';
				case type_data::tk_array:
					return L'\0';
				default:
					return L'\0';
				}
			}
		}
		bool as_boolean() const
		{
			if (data == NULL)
				return false;
			else
			{
				switch (data->type->get_kind())
				{
				case type_data::tk_real:
					return data->real_value != 0.0L;
				case type_data::tk_char:
					return data->char_value != L'\0';
				case type_data::tk_boolean:
					return data->boolean_value;
				case type_data::tk_array:
					return data->array_value.size() != 0;
				default:
					return false;
				}
			}
		}
		std::wstring as_string() const
		{
			if (data == NULL)
				return L"(VOID)";

			else
			{
				switch (data->type->get_kind())
				{
				case type_data::tk_real:
				{
					wchar_t buffer[128];
					long double isInt;
					if (modf(data->real_value, &isInt) == 0.0) {
						std::swprintf(buffer, L"%d", static_cast < int > (data->real_value));
					} else {
						std::swprintf(buffer, L"%Lf", data->real_value);
					}
					return std::wstring(buffer);
				}

				case type_data::tk_char:
				{
					std::wstring result;
					result += data->char_value;
					return result;
				}

				case type_data::tk_boolean:
					return (data->boolean_value) ? L"true" : L"false";

				case type_data::tk_array:
				{
					if (data->type->get_element()->get_kind() == type_data::tk_char)
					{
						std::wstring result;
						for (unsigned i = 0; i < data->array_value.size(); ++i)
							result += data->array_value[i].as_char();
						return result;
					}
					else
					{
						std::wstring result = L"[";
						for (unsigned i = 0; i < data->array_value.size(); ++i)
						{
							result += data->array_value[i].as_string();
							if (i != data->array_value.size() - 1)
								result += L",";
						}
						result += L"]";
						return result;
					}
				}
				case type_data::tk_object:
				{
					return L"Object";
				}
				default:
					return L"(INTERNAL-ERROR)";
				}
			}
		}
		unsigned length_as_array() const
		{
			return data->array_value.size();
		}
		value const & index_as_array(unsigned i) const
		{
			return data->array_value[i];
		}
		value & index_as_array(unsigned i)
		{
			return data->array_value[i];
		}
		type_data * get_type() const
		{

			return data->type;
		}

		void unique() const
		{
			if (data == NULL)
			{
				data = new body;
				data->ref_count = 1;
				data->type = NULL;
			}
			else if (data->ref_count > 1)
			{
				--(data->ref_count);
				data = new body(*data);
				data->ref_count = 1;
			}
		}

		//danger! called from the outside
		void overwrite(value const & source)
		{

			*data = *source.data;
			++(data->ref_count);
		}

	};

	class script_machine;

	typedef value(*callback)(script_machine * machine, int argc, value const * argv); //pointer to a function, returns value

	struct function
	{
		char const * name;
		callback func;
		unsigned arguments;
	};

	class script_type_manager
	{
	private:

		std::list <type_data> types;	//doubly linked list // 中身のポインタを使うのでアドレスが変わらないようにlist
		type_data * real_type;
		type_data * char_type;
		type_data * boolean_type;
		type_data * string_type;
		type_data * object_type;
	public:
		script_type_manager()
		{
			real_type = &* types.insert(types.end(), type_data(type_data::tk_real));
			char_type = &* types.insert(types.end(), type_data(type_data::tk_char));
			boolean_type = &* types.insert(types.end(), type_data(type_data::tk_boolean));
			string_type = &* types.insert(types.end(), type_data(type_data::tk_array, char_type));
			object_type = &* types.insert(types.end(), type_data(type_data::tk_object));
		}

		type_data * get_real_type()
		{
			return real_type;
		}

		type_data * get_char_type()
		{
			return char_type;
		}

		type_data * get_boolean_type()
		{
			return boolean_type;
		}

		type_data * get_string_type()
		{
			return string_type;
		}

		type_data * get_array_type(type_data * element)
		{
			for (std::list < type_data >::iterator i = types.begin(); i != types.end(); ++i)
			{
				if (i->get_kind() == type_data::tk_array && i->get_element() == element)
				{
					return &* i;
				}
			}
			return &* types.insert(types.end(), type_data(type_data::tk_array, element));
		}

		type_data * get_object_type()
		{
			return object_type;
		}

	};

	class script_engine
	{
	public:

		//error
		bool error;
		std::string error_message;
		int error_line;

		// intermediate code //中間コード
		enum command_kind //parser tokens
		{
			pc_assign, pc_assign_writable, pc_break_loop, pc_break_routine, pc_call, pc_call_and_push_result, pc_case_begin,
			pc_case_end, pc_case_if, pc_case_if_not, pc_case_next, pc_compare_e, pc_compare_g, pc_compare_ge, pc_compare_l,
			pc_compare_le, pc_compare_ne, pc_dup, pc_dup2, pc_loop_ascent, pc_loop_back, pc_loop_count, pc_loop_descent,
			pc_loop_if, pc_pop, pc_push_value, pc_push_variable, pc_push_variable_writable, pc_swap, pc_yield, pc_exit
		};

		struct block;

		struct code
		{
			command_kind command;
			int line;	//!!line in the source code
			value data;	//!!data used to push in pc_push_value

			union
			{
				struct
				{
					int level;	//environment for a variable in assign/push_variable					//assign/push_variableの変数の環境位置
					unsigned variable;	//index of the variable assign/push_variable					//assign/push_variableの変数のインデックス 
				};
				struct
				{
					block * sub;	//jump in call / call_and_push_result								//call/call_and_push_resultの飛び先
					unsigned arguments;	//the number of arguments in call/call_and_push_result			//call/call_and_push_resultの引数の数
				};
				struct
				{
					int ip;	//loop_back return destination												 //loop_backの戻り先
				};
			};

			code()
			{
			}

			code(int the_line, command_kind the_command) : line(the_line), command(the_command)
			{
			}

			code(int the_line, command_kind the_command, int the_level, unsigned the_variable) : line(the_line), command(the_command), level(the_level), variable(the_variable)
			{
			}

			code(int the_line, command_kind the_command, block * the_sub, int the_arguments) : line(the_line), command(the_command), sub(the_sub),
				arguments(the_arguments)
			{
			}

			code(int the_line, command_kind the_command, int the_ip) : line(the_line), command(the_command), ip(the_ip)
			{
			}

			code(int the_line, command_kind the_command, value const & the_data) : line(the_line), command(the_command), data(the_data)
			{
			}
		};

		enum block_kind
		{
			bk_normal, bk_loop, bk_sub, bk_function, bk_microthread
		};

		struct block //int level, arguments, string name, callback func, 
		{
			int level;
			int arguments;
			std::string name;
			callback func;
			lightweight_vector<code> codes;
			block_kind kind;

			block(int the_level, block_kind the_kind) : level(the_level), arguments(0), name(), func(NULL), codes(), kind(the_kind)
			{
			}
		};

		std::list <block> blocks;	//中身のポインタを使うのでアドレスが変わらないようにlist //doubly linked list
		block * main_block;
		std::map < std::string, block * > events;  //events are those like @Initialize and @MainLoop

		block * new_block(int level, block_kind kind)
		{
			return &*blocks.insert(blocks.end(), (block(level, kind))); //dereference blocks.insert return the address (returns the address of newly
																		//created block x(level,kind)
		}

		bool get_error()
		{
			return error;
		}

		std::string & get_error_message()
		{
			return error_message;
		}

		int get_error_line()
		{
			return error_line;
		}

		//compatibility and type

		script_type_manager * type_manager;

		script_type_manager * get_type_manager()
		{
			return type_manager;
		}

		type_data * get_real_type()
		{
			return type_manager->get_real_type();
		}

		type_data * get_char_type()
		{
			return type_manager->get_char_type();
		}

		type_data * get_boolean_type()
		{
			return type_manager->get_boolean_type();
		}

		type_data * get_array_type(type_data * element)
		{
			return type_manager->get_array_type(element);
		}

		type_data * get_string_type()
		{
			return type_manager->get_string_type();
		}

		type_data * get_object_type()
		{
			return type_manager->get_object_type();
		}

		void * data;	// space for the client //クライアント用空間 //not really needed. DirectX perhaps? 

		script_engine(script_type_manager * a_type_manager, std::string const & source, int funcc, function const * funcv);

		~script_engine()
		{
			blocks.clear();
		}

	};

	class script_machine
	{
	private:
		script_machine();
		script_machine(script_machine const & source);
		script_machine & operator = (script_machine const & source);

		script_engine * engine;

		bool error;
		std::string error_message;
		int error_line;

		typedef lightweight_vector < value > variables_t;
		typedef lightweight_vector < value > stack_t;

		struct environment
		{
			environment * pred, *succ;
			environment * parent;
			int ref_count;
			script_engine::block * sub;
			unsigned ip;
			variables_t variables; //vector of type value
			stack_t stack; //vector of type value
			bool has_result;
		};

		environment * first_using_environment;
		environment * last_using_environment;
		environment * first_garbage_environment;
		environment * last_garbage_environment;
		environment * new_environment(environment * parent, script_engine::block * b);
		void dispose_environment(environment * object);

		lightweight_vector < environment * > threads; //a vector of pointers to environments, called threads
		unsigned current_thread_index;
		bool finished;
		bool stopped;
		bool resuming;

		void yield()
		{
			if (current_thread_index > 0)
				--current_thread_index;
			else
				current_thread_index = threads.size() - 1;
		}

		void advance();


	public:
		script_machine(script_engine * the_engine);
		virtual ~script_machine();

		void run();
		void call(std::string event_name);
		void resume();

		void stop()
		{
			finished = true;
			stopped = true;
		}

		bool get_stopped()
		{
			return stopped;
		}

		bool get_resuming()
		{
			return resuming;
		}

		bool get_error()
		{
			return error;
		}

		std::string & get_error_message()
		{
			return error_message;
		}

		int get_error_line()
		{
			return error_line;
		}

		void raise_error(std::string const & message)
		{
			error = true;
			error_message = message;
			finished = true;
		}

		script_engine * get_engine()
		{
			return engine;
		}

		bool has_event(std::string event_name);

		int get_current_line();


	};
}

#endif