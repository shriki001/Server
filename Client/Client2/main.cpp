
#include <iostream>
#include "ChatBox.h"
#include "Client.h"
#include "PacketType.h"
#include <fstream>
#include <string>
#include <vector>


int main()
{
	///TODO: need to be static
	std::ifstream file;
	std::vector<std::string> serverStr;
	std::string tmp;
	file.open("server.txt");
	for (; getline(file, tmp);)
		serverStr.emplace_back(tmp);

	int port = stoi(serverStr.at(1));
	std::string name = "debug"; // take the name from user
	Client client(name);
	sf::Packet packet;
	sf::Socket::Status status;
	status = client.connect(serverStr.at(0), port);
	if (status != sf::Socket::Done)
	{
		std::cout << "Sorry we couldn't connect\n"
			"    try again later\n";
		return -1;
	}
	bool hit = true;
	sf::Vector2i vec{ 9,7 };
	client.send(INITIAL_NAME_DATA, name);
	client.receive();

	while (true)
	{
		sf::sleep(sf::seconds(1));
		client.send(COORDINATE, vec);
		client.receive();
		client.send(HIT_OR_NOT, hit);
		client.receive();
		client.receive();
	}
	std::cin.get();
}
