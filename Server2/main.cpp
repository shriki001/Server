#include <iostream>
#include "Server.h"
#include <string>
#include <fstream>

int main()
{
	std::ifstream file;
	std::string portStr;
	file.open("server.txt"); // open file to read the port number
	// file opening check
	if (!file.is_open())
		std::cerr << "Cannot open file for port number\n";
	// read port from file
	while (!file.eof())
		std::getline(file, portStr);
	file.close(); // close file
	int port = stoi(portStr); // cast port from string to int

	Server server(port); // setup server
	server.run(); // run server for good
	return EXIT_SUCCESS;
}
