// A regression test for PfffOptions
#include "config.h"
#include "PfffOptionManager.h"
#include "PfffOutputFormatter.h"
#include <string.h>

string compute_option_signature(const string& optstring) {
		// Split to argc & argv
		char optstring_c[500];
		strcpy(optstring_c, optstring.c_str());
		char argv_data[30][30];
		char* argv[30];
		int argc = 0;
		char* pch = strtok(optstring_c," ");
		while (pch != NULL) {
			strcpy(argv_data[argc], pch);
			argv[argc] = argv_data[argc];
			argc++;
			pch = strtok (NULL, " ");
		}
		// Create instance of PfffOptionManager
		PfffOptionManager optmgr;
		ostringstream null;
		ostringstream out;
		optmgr.cerr = &null;
		optmgr.TEST_MODE = true;
		
		// Compute the option signature (or the word "EXCEPTION")
		try {
			optmgr.init_from_cmdline_or_die(argc, argv);
			if (optmgr.help) out << "HELP";
			else {
                PfffOptionsSignature signature(&optmgr.options);
                signature.print(out);
            }
		}
		catch(string& s) {
			out << "EXCEPTION";
		}
		return out.str();
}
 
TEST_TWOFILEFIXTURE("TestPfffOptions.in", "TestPfffOptions.out", TestPfffOptions) {
	while(!eof_in()) {
	string optionstring = next_line_in();
 	string expected = next_line_out();
 	string actual = compute_option_signature(optionstring);
	if (!(expected == actual)) {
		cout << "---------------- TEST FAILURE -----------------" << endl;
		cout << "Option string:" << optionstring << endl;
		cout << "Expected: " << expected << endl;
		cout << "Actual:   " << actual << endl;
		cout << endl;
	}
		CHECK_EQUAL(expected, actual);//next_line_out(), compute_option_signature(next_line_in()));
	}
}

