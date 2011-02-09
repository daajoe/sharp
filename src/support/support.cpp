/*
 ============================================================================
 Name        : support.cpp
 Author      : Michael Morak
 Version     : 1.0
 Description : Error handling routines.
 Date        : 2007-11-07
 ============================================================================
 */
#include <errno.h>

#include <iostream>
#include <cstdlib>
#include <cstring>

using namespace std;

#include <support.hpp>

#define MAX_LENGTH 4096

namespace sharp
{
	
	/*
	 ============================================================================
	 Variable    : stimer
	 Description : A (global) timer instance
	 Date        : 2011-01-24
	 ============================================================================
	 */
	Timer stimer;
	
	/*
	 ============================================================================
	 Variable    : sProgramName
	 Description : A string that stores the program name (argv[0])
	 Date        : 2007-11-07
	 ============================================================================
	 */
	char sProgramName[MAX_LENGTH];
	
	/*
	 ============================================================================
	 Function    : InitializeErrorHandling
	 Description : Initializes the variables needed for the error handling
	               routines defined here.
	 Date        : 2007-11-07
	 Globals     : sProgramName
	 ============================================================================
	 */
	void InitializeErrorHandling(const char *programName)
	{
	    if(programName == NULL)
	    {
	        strncpy(sProgramName, "Error", MAX_LENGTH);
	    }
	    else
	    {
	        strncpy(sProgramName, programName, MAX_LENGTH);
	        sProgramName[MAX_LENGTH - 1] = '\0';
	    }
	
	    stimer.start();
	}
	
	/*
	 ============================================================================
	 Function    : PrintError
	 Description : Prints the given error message to the stderr stream and exits
	               the program with return code EXIT_FAILURE after calling 
	               the function stored in the freeResourcesFunction pointer.
	 Date        : 2007-11-07
	 Globals     : sProgramName
	 ============================================================================
	 */
	#ifdef DEBUG
	void _printError(int line, const char *file, const char *message)
	{
	    if(errno == 0)
	    	cerr << sProgramName << ": " << file << ":" << line << ": ERROR - " << message << endl;
	    else
	    	cerr << sProgramName << ": " << file << ":" << line << ": ERROR - " << message << " - " << strerror(errno) << endl;
	#else
	void _printError(const char *message)
	{
	    if(errno == 0)
	    	cerr << sProgramName << ": ERROR - " << message << endl;
	    else
	    	cerr << sProgramName << ": ERROR - " << message << " - " << strerror(errno) << endl; 
	#endif
	    exit(EXIT_FAILURE);
	}
	
	/*
	 TIMER class
	 */
	
	Timer::Timer()
	{
	        this->start();
	}
	
	Timer::~Timer() { }
	
	void Timer::start()
	{
	        getrusage(RUSAGE_SELF, &this->beginning);
	}
	
	pair<double, double> Timer::stop()
	{
	        double cpu, sys;
	
	        getrusage(RUSAGE_SELF, &this->end);
	
	        cpu = double(this->end.ru_utime.tv_sec - this->beginning.ru_utime.tv_sec)
	                + double(this->end.ru_utime.tv_usec - this->beginning.ru_utime.tv_usec) / 1000000.0;
	        sys = double(this->end.ru_stime.tv_sec - this->beginning.ru_stime.tv_sec)
	                + double(this->end.ru_stime.tv_usec - this->beginning.ru_stime.tv_usec) / 1000000.0;
	
	        return pair<double, double>(cpu, sys);
	}
	
	void Timer::printStop()
	{
		pair<double, double> time = this->stop();
	        cout << time.first;
	}

} // namespace sharp
