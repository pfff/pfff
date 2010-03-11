/**
 * OptionManager.h: Generic convenience class for command-line option management.
 * Provides some simple convenience routines over getopt_long.
 * Recommended usage by subclassing, see examples.
 *
 * Copyright: 2002, Konstantin Tretyakov.
 * License:   The terms of use of this software and its source code are defined by the MIT license.
 */
#ifndef __OptionManager_h__
#define __OptionManager_h__
#include<getopt.h> 
#include<iostream>
#include<map>
#include<string>
#include<sstream>
#include<vector>

using std::endl;
using std::map;
using std::ostream;
using std::string;
using std::stringstream;
using std::vector;

/**
 * Functor for converting each option from char* to its proper format.
 */
class OptionReader {
public:
	// Called before options are read to set the default value.
	virtual void prepare() = 0;
	// Return false on conversion/validation error
	virtual bool process(const char* value) = 0;
	// Returns a string describing the type expected by this option reader 
	// Used for error reporting in the context of "expecting %s".
	virtual const char* expected_type() = 0;
};

/**
 * OptionReader for LongInt type options.
 */
class LongIntOption: public OptionReader {
public:
	long* target_variable;
	long default_value;
	
	LongIntOption(long* target_variable, long default_value):
		target_variable(target_variable), default_value(default_value) {}
	 
	void prepare();
	bool process(const char* value);
	const char* expected_type();
};

/**
 * Same as LongIntOption but will not pass negative values.
 */
class PositiveLongIntOption: public OptionReader {
public:
	long* target_variable;
	long default_value;
	
	PositiveLongIntOption(long* target_variable, long default_value):
		target_variable(target_variable), default_value(default_value) {}
	 
	void prepare();
	bool process(const char* value);
	const char* expected_type();
};

/**
 * Same as LongIntOption but will has upper and lower bounds.
 */
class BoundedLongIntOption: public OptionReader {
public:
	long* target_variable;
	long default_value;
	long min_value;
	long max_value;
	char expected_type_str[50];
	
	BoundedLongIntOption(long* target_variable, long min_value, long max_value, long default_value):
		target_variable(target_variable), min_value(min_value),
		max_value(max_value), default_value(default_value) {
			sprintf(expected_type_str, "an integer between %d and %d", min_value, max_value);
	}

	void prepare();
	bool process(const char* value);
	const char* expected_type();
};

/**
 * OptionReader for Char* type options.
 */
class CharPtrOption: public OptionReader {
public:
	const char** target_variable;
	const char* default_value;
	
	CharPtrOption(const char** target_variable, const char* default_value):
		target_variable(target_variable), default_value(default_value) {}
	 
	void prepare();
	bool process(const char* value);
	const char* expected_type();
};

/**
 * Structure to keep information about each option.
 */
struct OptionDef {
	enum OptionType {
		OPTION_GROUP,
		UNPARAMETERIZED,
		PARAMETERIZED
	};

	OptionType type;
	const char* long_option_name; 
	char  short_option_name;
	int*  presence_counter;
	OptionReader* option_reader;
	const char* param_name;
	const char* description; // For OPTION_GROUP this is just its name, for other options, 
	                         // it's the text that will go in the help message.
	
	OptionDef(OptionType type, const char* long_option_name, char short_option_name, 
	          int* presence_counter, OptionReader* option_reader, const char* param_name, const char* description):
		type(type),
		long_option_name(long_option_name),
		short_option_name(short_option_name),
		presence_counter(presence_counter),
		option_reader(option_reader),
		param_name(param_name),
		description(description) {}
	
	/**
	 * Set default value.
	 */
	inline void prepare() {
		if (presence_counter != NULL) *presence_counter = 0;
		if (type == PARAMETERIZED && option_reader != NULL) option_reader->prepare();
	}
	
	/**
	 * Return struct option for use with getopt_long. Makes no sense for OPTION_GROUPs.
	 */
	inline option get_option_struct() {
		option o;
		o.name = long_option_name;
		o.has_arg = (type == UNPARAMETERIZED)? no_argument : required_argument;
		o.flag = NULL;
		o.val = short_option_name;
		return o;
	}
};


/**
 * A convenience class for managing command-line options.
 * Use by subclassing (see examples).
 * Note - the class does not free memory after itself (but you shouldn't worry too much).
 */
struct OptionManager {
protected:
	vector<OptionDef> options;
	map<char, int>    char_to_option; // Maps short option to index in options.
	int num_options;
	int num_parameterized;
	int max_option_len;

public:
	ostream* cerr; 	// Override this to disable output (useful for testing)
	vector<char*> parameters; // This is where the "non-option" parameters go.
	
	OptionManager():
		options(), char_to_option(), num_options(0), num_parameterized(0), max_option_len(0), parameters(), cerr(&std::cerr) {}
	
	/**
	 * Initialize all options and read from cmdline. Return false on failure. Reports errors on stderr.
	 */
	bool read_from_cmdline(int argc, char* const argv[]);

	/**
	 * Outputs descriptions of all the options
	 */
	void print_option_help(ostream& out);
	
protected:
	/**
	 * Used on OptionManager initialization, adds a "group"-type OptionDef to the vector of available options.
	 */
	void add_group(const char* name);
	
	/**
	 * Used on OptionManager initialization, adds a "unparameterized"-type OptionDef to the vector of available options.
	 */
	void add_unparameterized(const char* long_option, char short_option, int* presence_counter, const char* description);
	
	/**
	 * Used on OptionManager initialization, adds a "parameterized"-type OptionDef to the vector of available options.
	 */
	void add_parameterized(const char* long_option, char short_option, int* presence_counter, OptionReader* option_reader, const char* param_name, const char* description);
	
	/**
	 * Returns a pointer to OptionDef corresponding to a given short_option char or NULL.
	 */
	OptionDef* find_option(char c);

};

#endif

