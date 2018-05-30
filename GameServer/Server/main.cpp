#include <iostream>
#include "Server.h"
#include <string>
#include <fstream>

int main()
{
	std::ifstream file;
	std::string portStr;
	
	try // file opening check
	{
		file.open("port.txt"); // open file to read the port number
		if (!file.is_open())	
			throw std::runtime_error("Cannot open file for port number\n");	
	}
		catch(std::exception)
		{
			exit(EXIT_FAILURE);
		}

	// read port from file
	while (!file.eof())
		std::getline(file, portStr);
	
	file.close(); // close file
	int port = stoi(portStr); // cast port from string to int
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	Server server(port); // setup server
	server.run(); // run server for good
	return EXIT_SUCCESS;
}