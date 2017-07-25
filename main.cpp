#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <fstream>
#include <string>
#include <regex>
#include <map>
#include <iterator>

#define MAXBUF 1024

/* The Id, the ip address and the listening port of the currently running
process.
*/
int current_process_Id;
std::string current_process_address;
int current_process_port;

int nClient;
int nFDList[1000];

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

void *clientHandler (void *ptr) {

	int nDesc = *((int*) ptr);

	printf("Client handler activated for FD %d\n", nDesc);

	char buff[MAXBUF];

	int len;

	while (1) {
		if ((len = read(nDesc, buff, MAXBUF)) > 0) {
			printf("%s\n", buff);

			int i;
			for (i = 0; i < nClient; i++) {
				if (nDesc != nFDList[i])
					write (nFDList[i], buff, strlen(buff) + 1);
			}
		}
	}

}

/*
Connect to a process.
*/
int connect(char *host, process p) {
	struct hostent *hp;
	unsigned int alen;
	struct sockaddr_in myaddr;
	struct sockaddr_in servaddr;
	int fd = p.pid;

	printf("conn(host=\"%s\", port=\"%d)\n", host, p.port);

	if ((fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		 printf("cannot create socket");
		 return 0;
	}

	memset((char *) &myaddr, 0, sizeof(myaddr));
	myaddr.sin_family = AF_INET;
	myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	myaddr.sin_port = htons(current_process_port);

	if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
		printf("bind failed");
		return 0;
	}

	memset((char*)&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(p.port);

	hp = gethostbyname(host);
	if (!hp) {
		fprintf(stderr, "could not obtain address of %s\n", host);
		return 0;
	}

	memcpy((void *)&servaddr.sin_addr, hp->h_addr_list[0], hp->h_length);

	if (connect(fd, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
		printf("connect failed");
		return 0;
	}
	return fd;
}

void disconnect(int &fd) {
	printf("Disconnecting\n");
	shutdown(fd, 2);
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
	char buf[MAXBUF];
	nClient = 0;

  int n, s, ns, len;
	struct sockaddr_in name;
	int nClientFileDescriptor = -1;
	const char *host = "localhost";

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

	if ((s = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("socket");
		exit (1);
	}

	int flag = 1;
	setsockopt(s,
		IPPROTO_TCP,
		TCP_NODELAY,
		(char *) &flag,
		sizeof(int));

	memset (&name, 0, sizeof (struct sockaddr_in));
	name.sin_family = AF_INET;
	name.sin_port = htons (current_process_port);
	len = sizeof (struct sockaddr_in);

	inet_pton(AF_INET, current_process_address.c_str(), &name.sin_addr);
	//memcpy(&name.sin_addr, &current_process_address, sizeof (long));

	if (bind (s, (struct sockaddr *) &name, len) < 0) {
		printf("bind\n");
		exit(1);
	}

	while (1) {
		if (listen(s, 5) < 0) {
			printf("listen");
			exit(1);
		}
		if ((nClientFileDescriptor = accept(s, (struct sockaddr *) &name, (socklen_t *) &len)) < 0) {
			printf("accept");
			exit(1);
		}

		nFDList[nClient++] = nClientFileDescriptor;

		pthread_t thread;

		pthread_create(&thread, NULL, clientHandler, (void*) &nClientFileDescriptor);
	}

	if (current_process_Id != 1) {
		for (auto it = process_list.begin(); it != process_list.find(current_process_Id); ++it) {
			if (it->first != current_process_Id) {
				std::cout << "pid: " << it->second.pid << std::endl;
				if (!(nClientFileDescriptor = connect((char*) host, it->second))) {
					exit(1);
				}
                                printf("Connected Successfully");
			}
		}
	}

	return 0;
}
