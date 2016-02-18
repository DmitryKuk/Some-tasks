#include <iostream>
#include <sstream>
#include <string>
#include <map>
#include <stdexcept>
#include <regex>

using namespace std::literals;



template<class T>
std::string class_name()
{
	return "<unknown type>"s;
}


template<>
std::string class_name<std::string>()
{
	return "STRING"s;
}


template<>
std::string class_name<int>()
{
	return "INT"s;
}



template<class T>
void set_value(const std::string &opt_name, T &res, std::string &&value)
{
	std::stringstream stream{std::move(value)};
	if (!(stream >> res))
		throw std::logic_error{"Value for option \""s + opt_name + "\" not provided, but required"s};
}


template<>
void set_value<std::string>(const std::string &opt_name, std::string &res, std::string &&value)
{
	if (value.empty())
		throw std::logic_error{"Value for option \""s + opt_name + "\" not provided, but required"s};
	res = std::move(value);
}



class option
{
public:
	option(const std::string &name, bool required = false):
		name_{name},
		required_{required},
		is_set_{false}
	{}
	
	
	virtual ~option() = default;
	
	
	inline const std::string & name() const
	{ return this->name_; }
	
	inline bool required() const
	{ return this->required_; }
	
	inline bool is_set() const
	{ return this->is_set_; }
	
	
	virtual void print(bool print_value = false, std::ostream &stream = std::cout)
	{
		stream << ((this->required())? ' ': '[') << this->name() << ((this->required())? ' ': ']');
		
		if (print_value && this->is_set())
			stream << " <set>";
	}
	
	
	virtual bool value_required() const
	{ return false; }
	
	
	// Called with empty string, if value_required() returns false
	virtual void set_value(std::string && /* value */)
	{ this->is_set_ = true; }
private:
	std::string name_;
	bool required_;
protected:
	bool is_set_;
};	// class option



template<class T>
class option_with_value: public option
{
public:
	using option::option;
	
	
	inline const T & value() const
	{ return this->value_; }
	
	inline T & value()
	{ return this->value_; }
	
	
	virtual void print(bool print_value = false, std::ostream &stream = std::cout) override
	{
		stream
			<< ((this->required())? ' ': '[') << this->name()
			<< ' ' << class_name<T>() << ((this->required())? ' ': ']');
		
		if (print_value && this->is_set())
			stream << " = \"" << this->value() << '\"';
	}
	
	
	virtual bool value_required() const override
	{ return true; }
	
	
	virtual void set_value(std::string &&value) override
	{
		::set_value<T>(this->name(), this->value_, std::move(value));
		this->is_set_ = true;
	}
private:
	T value_;
};	// class option_with_value



template<bool AutoHelp = true>
class option_set
{
public:
	option_set()
	{}
	
	
	// Returns true, if option successfully added
	template<class OptionType = option, class... Args>
	bool add(Args &&... args)
	{
		auto p = std::make_unique<OptionType>(std::forward<Args>(args)...);
		auto name = p->name();
		return this->options_.emplace(std::move(name), std::move(p)).second;
	}
	
	
	
	void print(bool print_values = false, std::ostream &stream = std::cout)
	{
		for (const auto &p: this->options_) {
			p.second->print(print_values, stream);
			stream << std::endl;
		}
	}
	
	
	
	void parse(int argc, char **argv)
	{
		static const std::regex
			key_eq_val{"--([^[:space:]]+)=(.*)"s},	// [1]: key, [2]: value
			key{"--([^[:space:]]+)"s};				// [1]: key
		
		
		// Parsing arguments
		std::cmatch m;
		
		int i = 1;
		while (i < argc) {
			if (std::regex_match(argv[i], m, key_eq_val)) {
				auto &opt = this->get_option(m.str(1));
				if (opt.value_required()) {
					opt.set_value(m.str(2));
				} else {
					if (m.length(2) != 0)
						throw std::logic_error{"Value for option \""s + m.str(1) + "\" provided, but not expected"s};
					opt.set_value(std::string{});
				}
			} else if (std::regex_match(argv[i], m, key)) {
				auto &opt = this->get_option(m.str(1));
				if (opt.value_required()) {
					if (i + 1 < argc) {
						++i;
						opt.set_value(argv[i]);
					} else {
						throw std::logic_error{"Value for option \""s + m.str(1) + "\" not provided, but required"s};
					}
				} else {
					opt.set_value(std::string{});
				}
			}
			
			++i;
		}
		
		
		// Checking arguments
		for (const auto &p: this->options_)
			if (p.second->required() && !p.second->is_set())
				throw std::logic_error{"Value for option \""s + p.second->name() + "\" not provided, but required"s};
	}
	
	
	option & get_option(std::string &&key)
	{
		if (AutoHelp) {
			if (key == "help") {
				std::cerr << "Options list (by autohelper):" << std::endl;
				this->print(false, std::cerr);
			}
		}
		
		
		try {
			return *this->options_.at(key);
		} catch (const std::out_of_range &) {
			throw std::out_of_range("Option \""s + key + "\" not found"s);
		}
	}
private:
	std::map<std::string, std::unique_ptr<option>> options_;
};	// class option_set



int main(int argc, char **argv)
{
	option_set<> s;	// Use option_set<false> to disable autohelper
	
	
	s.add<option>("help");
	s.add<option>("hi", true);
	s.add<option_with_value<int>>("ival");
	s.add<option_with_value<std::string>>("sval", true);
	
	
	std::cout << "Options list before parsing:" << std::endl;
	s.print();
	std::cout << std::endl;
	
	try {
		s.parse(argc, argv);
	} catch (const std::exception &e) {
		std::cout << e.what() << '.' << std::endl;
		return 1;
	}
	
	std::cout << std::endl << "Options list after parsing:" << std::endl;
	s.print(true);
	
	return 0;
}
