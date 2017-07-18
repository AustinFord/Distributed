#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <regex>
#include <map>
#include <iterator>

int current_process_Id;
std::string current_process_address;
int current_process_port;

std::ifstream setupFile("setup.txt");

struct process {
	int pid;
	std::string ip;
	int port;
};

std::map<int, process> process_list;

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

void setup(int argument)
{
	current_process_Id = argument;
	setup_list_of_processes();
	std::map<int, process>::iterator it = process_list.begin();

	it = process_list.find(current_process_Id);

	current_process_port = it->second.port;
	current_process_address = it->second.ip;

}
int main(int argc, char **argv)
{
	int arg = atoi(argv[1]);
	if(arg <=0 || arg > 10)
	{
		std::cout <<"ID: "<< arg <<" is not allowed" << std::endl;
		std::cout << "Please enter an ID between 1 and 10" << std::endl;
		return 0;
	}
	setup(arg);
	std::cout << "I am number: " << current_process_Id << std::endl;
	std::cout << "My address is: " << current_process_address << std::endl;
	std::cout << "I am listening on port: " << current_process_port << std::endl;
	return 0;
}
