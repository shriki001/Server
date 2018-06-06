#include <iostream>
#include "GameServer.h"
#include "ChatServer.h"
#include <string>
#include <fstream>

int main()
{
	std::ifstream file;
	std::vector<std::string> portStr;

	try // file opening check
	{
		file.open("port.txt"); // open file to read the port number
		if (!file.is_open())
			throw std::runtime_error("Cannot open file for port number\n");
	}
	catch (std::exception)
	{
		exit(EXIT_FAILURE);
	}
	std::string tmp;
	// read port from file
	while (!file.eof())
	{
		std::getline(file, tmp);
		portStr.emplace_back(std::move(tmp));
	}
	file.close(); // close file
	int GamePort = stoi(portStr.at(0)); // cast port from string to int
	int ChatPort = stoi(portStr.at(1));
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
	
	GameServer game(GamePort); // setup server
	ChatServer chat(ChatPort); // setup server
	
	static sf::Thread thread([&]()
	{
		game.run(); // run game server for good
	});
	thread.launch();
	
	chat.run(); // run chat server for good

	return EXIT_SUCCESS;
}