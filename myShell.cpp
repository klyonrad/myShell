#include <unistd.h>
#include <string>
#include <sstream>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string.h> // because of strcmp
#include <iostream>
#include <vector>
#include <sys/wait.h>
#include <signal.h>


using std::cin;
using std::cout;
using std::endl;

pid_t pid = 0;
bool shouldRunInBackground = false; // global >.< because I don't wanna touch my structure anymore.

/* from http://stackoverflow.com/a/236803/1796645 */
std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}
std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

bool parse(std::string inputline, char *arglist[]) // return false if string is empty
{
	std::vector<std::string> splittedstrings;
	splittedstrings = split(inputline, ' ');
	if (splittedstrings.size() == 0)
		return false;
	unsigned int i = 0;
	while (i < splittedstrings.size() ) 
	{
		arglist[i] = /*necessary casting from const to non-const*/ (char*) splittedstrings[i].c_str();
		/* for debugging: cout << arglist[i] << "\n"; */
		i++;
	}
	
	if (*arglist[i-1] == '&') {
		shouldRunInBackground = true;
		// cout << "& detected";
		arglist[i-1] = '\0';
		}
	arglist[i] = '\0'; // "the array of pointers _must_ be terminated by a Null pointer". Should this be = NULL?
	return true;
}

int main(void)
{
	std::string inputline;
	char *arglist[64]; // arbitrary size. Don't wanna deal with that issue right now. Yeah I know it's bad style.
	
	cout << "Hello. This is myShell." << endl;
   	while (true) {
   		shouldRunInBackground = false;
   		if (getuid()) cout << "not root :( $ ";
   		else cout << "yeah root. Plz don't use this # ";
		std::getline(std::cin, inputline); // for the whole line. cin >> sucks.
		cout << "\n";

		if (parse (inputline, arglist) == false) break; // exit when line is empty		
		
		char exitcommand[] = "logout"; // for some reason it didn't work the other way; so I used strcmp
		if ( !strcmp (arglist[0], exitcommand)) 
			return 0;
				
		pid = fork ();	
		if (pid == 0) // child process
			execvp( (const char*) arglist[0], (char * const*) arglist);
			
		else if (pid > 0) {
			if (shouldRunInBackground == false)
			{
				int waitstatus;
				waitpid (pid, &waitstatus, 0);
			}
			else cout << "[" << pid << "] sent to background." << endl;	
		}
	}	
	return 0;
}
