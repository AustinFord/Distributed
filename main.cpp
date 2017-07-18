#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <regex>
#include <map>
#include <iterator>

/* The Id, the ip address and the listening port of the currently running
process.
*/
int current_process_Id;
std::string current_process_address;
int current_process_port;

/*
The file that is being read in to set up all the processes going to be ran.
*/
std::ifstream setupFile("setup.txt");

/* structure to encapsulate the three parts of a process: the process ID, the
Ip address where its located, and the port number it is listening on.
*/
struct process {
	int pid;
	std::string ip;
	int port;
};
/*
The map that will hold all the Process structures read in from the setup text
file.
*/
std::map<int, process> process_list;
/*
function that reads in the text file, breaks up the lines of files into the
process id, the ip address, and the listening port of all the processes.
*/
void setup_list_of_processes()
{
	int pid_to_add;
	std::string address_to_add;
	int port_to_add;

	setupFile >> pid_to_add >> address_to_add >> port_to_add;
	process newProcess = {pid_to_add, address_to_add, port_to_add};
	process_list.insert(std::make_pair(pid_to_add, newProcess));

	while(!setupFile.eof())
	{
		setupFile >> pid_to_add >> address_to_add >> port_to_add;
		process newProcess = {pid_to_add, address_to_add, port_to_add};
		process_list.insert(std::make_pair(pid_to_add, newProcess));
	}
	setupFile.close();
}
/* Function that setups up the current process with the ip address and port
associated with the process id.

@Param (int) argument - the argument given in the command line that will used
as the process id.
*/
void setup(int argument)
{
	current_process_Id = argument;
	setup_list_of_processes();
	std::map<int, process>::iterator it = process_list.begin();

	it = process_list.find(current_process_Id);

	current_process_port = it->second.port;
	current_process_address = it->second.ip;

}

/*
The argument given to the main function from the command line is the process id
we want to give to this current process running the program. For instance, we
would enter 7 into the command line when executing the program like ./a.out 7
and the process executing will then be process number 7. The program would then
look up what address and port is associated with process number 7 from the setup
file and assign the current processes address and port to those found in the
file.
*/
int main(int argc, char **argv)
{
	/*
	error checking to make sure the argument given is actually in the range of
	processes we want to execute. argv[1] is where the actual argument from
	the command line is. for instance when we do ./a.out 2, argv[1] is where the 2
	is.
	*/
	int arg = atoi(argv[1]);
	if(arg <=0 || arg > 10)
	{
		std::cout <<"ID: "<< arg <<" is not allowed" << std::endl;
		std::cout << "Please enter an ID between 1 and 10" << std::endl;
		return 0;
	}
	/*
	set up the current process with the process id given in the command line.
	*/
	setup(arg);

	/*
	Output letting the user know that the process is set up and what process
	it is.
	*/
	std::cout << "I am number: " << current_process_Id << std::endl;
	std::cout << "My address is: " << current_process_address << std::endl;
	std::cout << "I am listening on port: " << current_process_port << std::endl;
	return 0;
}
