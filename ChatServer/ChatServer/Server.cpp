#include "Server.h"
#include "PacketType.h"
#include "SFML/Graphics/RenderWindow.hpp"
#include <iostream>

Server::Server(unsigned short port)
{
	listner.listen(port);
	listner.setBlocking(false);
	printTime();
	std::cout << "Chat Server Start\n";
}


void Server::printTime()
{
	std::string nowTime;
	time(&rawtime);
	timeinfo = localtime(&rawtime);
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 14);
	nowTime = asctime(timeinfo);
	nowTime = nowTime.substr(0, nowTime.length() - 1);
	nowTime.append(": ");
	std::cout << nowTime;
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}

void Server::handlePackets()
{
	//handle incoming data
	for (Clients::iterator it = clients.begin(); it != clients.end();)
	{
		sf::Packet packet;
		sf::Socket::Status status = it->first->receive(packet);

		switch (status)
		{
		case sf::Socket::Done:
			PacketType type;
			packet >> type;
			if (type == NAME)
			{
				//store the name
				packet >> it->second;
				printTime();
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
				std::cout << it->second << " has joined\n";
				broadCast(NAME, it->second + " has joined\n");
			}
			else if (type == GMSG)
			{
				std::string msg;
				packet >> msg;
				broadCast(GMSG, it->second + ": " + msg);
			}
			++it;
			break;

		case sf::Socket::Disconnected:
			printTime();
			SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
			std::cout << it->second << " has been disconnected\n";
			broadCast(GMSG, it->second + " has been disconnected\n");
			it = clients.erase(it);
			break;

		default:
			++it;
		}
	}
}


void Server::broadCast(PacketType type, const std::string & msg)
{
	for (Clients::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		sf::Packet pack;
		pack << type << msg;
		it->first->send(pack);
	}
}




void Server::run()
{
	sf::Thread thread([&]() // server can send massage to all clients
	{
		while (true)
		{
			std::string s;
			std::getline(std::cin, s);
			broadCast(SMSG, "SERVER: " + s);
		}
	});
	thread.launch();

	sf::TcpSocket * nextClient = nullptr;
	while (1)
	{
		//Handle newcoming clients
		if (nextClient == nullptr)
		{
			nextClient = new sf::TcpSocket;
			nextClient->setBlocking(false);
		}
		if (listner.accept(*nextClient) == sf::Socket::Done)
		{
			clients.insert(std::make_pair(nextClient, ""));
			nextClient = nullptr;
		}
		handlePackets();

	}
}