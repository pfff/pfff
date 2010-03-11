/**
 * Copyright: 2002, Konstantin Tretyakov.
 * License:   The terms of use of this software and its source code are defined by the MIT license.
 */
#include "OptionManager.h"
#include <string.h>

#include <iostream>
#include <errno.h>
#include <cstdlib>
using namespace std;

// -------------- LongIntOption --------------
void LongIntOption::prepare() {
    *target_variable = default_value;
}

bool LongIntOption::process(const char* value) {
    char* endptr;
    errno = 0;
    *target_variable = strtol(value, &endptr, 0);
    return (errno == 0) && (*endptr == '\0');
}

const char* LongIntOption::expected_type() { return "an integer < 2147483647"; }

// -------------- PositiveLongIntOption --------------
void PositiveLongIntOption::prepare() {
    *target_variable = default_value;
}

bool PositiveLongIntOption::process(const char* value) {
    char* endptr;
    errno = 0;
    *target_variable = strtol(value, &endptr, 0);
    return (errno == 0) && (*endptr == '\0') && (*(target_variable) > 0);
}

const char* PositiveLongIntOption::expected_type() { return "a positive integer < 2147483647"; }

// -------------- BoundedLongIntOption --------------
void BoundedLongIntOption::prepare() {
    *target_variable = default_value;
}

bool BoundedLongIntOption::process(const char* value) {
    char* endptr;
    errno = 0;
    *target_variable = strtol(value, &endptr, 0);
    return (errno == 0) && (*endptr == '\0') && (*(target_variable) >= min_value) && (*(target_variable) <= max_value);
}

const char* BoundedLongIntOption::expected_type() { return expected_type_str; }

// -------------- CharPtrOption --------------
void CharPtrOption::prepare() {
    *target_variable = default_value;
}

bool CharPtrOption::process(const char* value) {
    *target_variable = value;
    return (value != NULL);
}

const char* CharPtrOption::expected_type() { return "a string"; }


// -------------- OptionManager --------------

/**
 * Initialize all options and read from cmdline. Return false on failure. Reports errors on stderr.
 */
bool OptionManager::read_from_cmdline(int argc, char* const argv[]) {
    // Initialize option values and prepare getopt_long parameters
    option long_options[num_options+1]; 
    memset(long_options, 0, sizeof(option)*(num_options+1));
    char short_options[num_options+num_parameterized+1]; short_options[num_options+num_parameterized] = 0;
    int offs_long = 0; 
    int offs_short = 0;
    for (vector<OptionDef>::iterator o = options.begin(); o != options.end(); o++) {
        // Init option value to default
        o->prepare();
        // Prepare getopt_long inputs
        if (o->type != OptionDef::OPTION_GROUP) {
            long_options[offs_long++] = o->get_option_struct();
            short_options[offs_short++] = o->short_option_name;
            if (o->type == OptionDef::PARAMETERIZED) short_options[offs_short++] = ':';
        }
    }

    // Parse via getopt_long
    optind = 0;
    int option_index;
    int option_chr;
    while (true) {
        option_chr = getopt_long(argc, argv, short_options, long_options, &option_index);
        if (option_chr == EOF) break;
        switch (option_chr) {
            case ':': // Missing parameter (contrarily to the manual this error code is never returned)
            case '?': // Unrecognized option
                // Getopt will automatically tell which option it is on the stderr
                return false;
                break;
            default: 
                OptionDef* o = find_option(option_chr);
                if (o == NULL) {// Some other generic mistake that must never happen in fact
                    *cerr << "Unexpected error for option " << (char)option_chr << endl;
                    return false;
                }
                else {
                    if (o->presence_counter != NULL) (*(o->presence_counter))++;
                    if (o->option_reader != NULL) {
                        if (!o->option_reader->process(optarg)) {
                            *cerr << "Error: Invalid value '" << optarg << 
                            "' for option '" << o->long_option_name << 
                            "'. Expecting " << o->option_reader->expected_type() << "." << endl;
                            return false;
                        }
                    }
                }
        }
    }

    while(optind < argc) {
        parameters.push_back(argv[optind++]);
    }
    return true;
}

/**
 * Outputs descriptions of all the options
 */
void OptionManager::print_option_help(ostream& out) {
    for (vector<OptionDef>::iterator o = options.begin(); o != options.end(); o++) {
        stringstream ss(string(o->description));
        char line[512];
        switch(o->type) {
            case OptionDef::OPTION_GROUP:
                out << o->description << ":" << endl;
                break;
            case OptionDef::UNPARAMETERIZED:
                out << "  " << "-" << o->short_option_name << endl;
                out << "  " << "--" << o->long_option_name << ": ";
                for (int i = strlen(o->long_option_name); i < max_option_len; i++) out << ' ';
                ss.getline(line, 512);
                out << line << endl;
                while (!ss.eof()) {
                    ss.getline(line, 512);
                    for (int i = 0; i < (6+max_option_len); i++) out << ' ';
                    out << line << endl;
                }
                out << endl;
                break;
            case OptionDef::PARAMETERIZED:
                out << "  " << "-" << o->short_option_name << ' ' << o->param_name << endl;
                out << "  " << "--" << o->long_option_name << ' ' << o->param_name << ": ";
                for (int i = (strlen(o->long_option_name) + strlen(o->param_name) + 1); i < max_option_len; i++) out << ' ';
                ss.getline(line, 512);
                out << line << endl;
                while (!ss.eof()) {
                    ss.getline(line, 512);
                    for (int i = 0; i < (6+max_option_len); i++) out << ' ';
                    out << line << endl;
                }
                out << endl;
                break;
        }
    }
}

/**
 * Used on OptionManager initialization, adds a "group"-type OptionDef to the vector of available options.
 */
void OptionManager::add_group(const char* name) {
    options.push_back(OptionDef(OptionDef::OPTION_GROUP, NULL, 0, NULL, NULL, NULL, name));
}

/**
 * Used on OptionManager initialization, adds a "unparameterized"-type OptionDef to the vector of available options.
 */
void OptionManager::add_unparameterized(const char* long_option, char short_option, int* presence_counter, const char* description) {
    num_options++;
    int l = long_option == NULL ? 0 : strlen(long_option);
    max_option_len = max_option_len >= l ? max_option_len : l;
    if (char_to_option.find(short_option) != char_to_option.end()) {
        *cerr << "Error: Two options have the same short option character " << short_option << ". Check your code!" << endl;
    }
    char_to_option[short_option] = options.size();
    options.push_back(OptionDef(OptionDef::UNPARAMETERIZED, long_option, short_option, presence_counter, NULL, NULL, description));
}

/**
 * Used on OptionManager initialization, adds a "parameterized"-type OptionDef to the vector of available options.
 */
void OptionManager::add_parameterized(const char* long_option, char short_option, int* presence_counter, OptionReader* option_reader, const char* param_name, const char* description) {
    num_options++; num_parameterized++;
    int l = long_option == NULL ? 0 : strlen(long_option) + strlen(param_name) + 1;
    max_option_len = max_option_len >= l ? max_option_len : l;
    if (char_to_option.find(short_option) != char_to_option.end()) {
        *cerr << "Error: Two options have the same short option character " << short_option << ". Check your code!" << endl;
    }
    char_to_option[short_option] = options.size();
    options.push_back(OptionDef(OptionDef::PARAMETERIZED, long_option, short_option, presence_counter, option_reader, param_name, description));
}

/**
 * Returns a pointer to OptionDef corresponding to a given short_option char or NULL.
 */
OptionDef* OptionManager::find_option(char c) {
    map<char,int>::iterator o = char_to_option.find(c);
    if (o == char_to_option.end()) return NULL;
    else return &(options[o->second]);
}

