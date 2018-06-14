#include "ChatServer.h"
#include "PacketType.h"
#include <iostream>

ChatServer::ChatServer(unsigned short port)
{
	sf::sleep(sf::milliseconds(50));
	listner.listen(port);
	listner.setBlocking(false);
	printTime();
	std::cout << "Chat Server Start\n";
}


void ChatServer::printTime()
{
	sf::sleep(sf::milliseconds(100));
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

void ChatServer::handlePackets()
{
	//handle incoming data
	for (Clients::iterator it = clients.begin(); it != clients.end();)
	{
		sf::Packet packet;
		sf::Socket::Status status = it->first->receive(packet);

		switch (status)
		{
		case sf::Socket::Done:
			PacketChat type;
			packet >> type;
			if (type == NAME)
			{
				//store the name
				packet >> it->second;
				printTime();
				SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
				std::cout << it->second << " has join To Chat\n";
				broadCast(NAME, it->second + " has join\n");
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
			std::cout << it->second << " has been disconnect From Chat\n";
			broadCast(GMSG, it->second + " has been disconnect\n");
			it = clients.erase(it);
			break;

		default:
			++it;
		}
	}
}


void ChatServer::broadCast(PacketChat type, const std::string & msg)
{
	for (Clients::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		sf::Packet pack;
		pack << type << msg;
		it->first->send(pack);
	}
}


void ChatServer::run()
{
	std::shared_ptr<sf::TcpSocket> nextClient;
	while (true)
	{
		//Handle newcoming clients
		if (!nextClient)
		{
			nextClient = std::make_shared<sf::TcpSocket>();
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