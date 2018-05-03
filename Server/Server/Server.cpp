#include "Server.h"
#include "PacketType.h"
#include <iostream>

/*
*	Constructor to set up the connection socket 
*/

Server::Server(int port)
{
	m_listner.listen(port);
	m_listner.setBlocking(false);
}

/*
 *	this function in charge of received new client and if 2 clients are connected 
 *	they can send and received messages
*/

void Server::run()
{
	std::shared_ptr<sf::TcpSocket> nextClient = nullptr;
	while (true)
	{
		//Handle new coming clients
		if (!nextClient)
		{
			nextClient = std::make_shared<sf::TcpSocket>();
			nextClient->setBlocking(false);
		}
		if (m_listner.accept(*nextClient) == sf::Socket::Done)
		{
			m_clients.emplace_back(std::move(std::make_pair(nextClient, "")));
			nextClient = nullptr;
		}
		if (m_clients.size() == ACCEPT_CLIENTS)
			handlePackets();
	}
}

/*
*	this function in charge of received and send messages between 2 clients
*/

void Server::handlePackets()
{
	std::string msg;
	PacketType MsgType;
	sf::Packet packet;
	sf::Vector2i vec;
	bool hit;
	
	while (true)
	{
		if (m_clients.size() == 1)
		{
			std::cout << m_clients.at(0).second + " has been disconnected\n";
			sf::sleep(sf::seconds(1));
			m_clients.clear();
		}
		if (m_clients.size() == 0)
			break;
		for (size_t i = 0; i < m_clients.size(); ++i)
		{
			sf::Socket::Status status = m_clients.at(i).first->receive(packet);

			switch (status)
			{
			case sf::Socket::Done:
				packet >> MsgType;
				if (MsgType == INITIAL_NAME_DATA)
				{
					packet >> msg;
					m_clients.at(i).second = msg;
					std::cout << msg + " has joined\n";
					broadCast(INITIAL_NAME_DATA, msg + " has joined\n", !i);
				}
				else if (MsgType == COORDINATE)
				{
					packet >> vec.x >> vec.y;
					broadCast(COORDINATE, vec, !i);
				}
				else if (MsgType == HIT_OR_NOT)
				{
					packet >> hit;
					broadCast(HIT_OR_NOT, hit, !i);
				}
				break;
			case sf::Socket::Disconnected:
				std::cout << m_clients.at(i).second + " has been disconnected\n";
				broadCast(SERVER_MSG, m_clients.at(i).second + " has been disconnected\n", !i);
				m_clients.erase(m_clients.begin() + i);
				break;
			default:
				++i; 
			}
		}
	}
}

/*
*	this function in charge of send message
*/

void Server::broadCast(PacketType MsgType, const std::string &msg, int index)
{
	sf::Packet pack;
	pack << MsgType << msg;
	m_clients.at(index).first->send(pack);
	sf::sleep(sf::seconds(1));
}

/*
*	this function in charge of send vector of coordinates
*/

void Server::broadCast(PacketType MsgType, const sf::Vector2i vec, int index)
{
	sf::Packet pack;
	pack << MsgType << vec.x << vec.y;
	m_clients.at(index).first->send(pack);
}

/*
*	this function in charge of send if there are hit in the coordinate
*/

void Server::broadCast(PacketType MsgType, const bool hit, int index)
{
	sf::Packet pack;
	pack << MsgType << hit;
	m_clients.at(index).first->send(pack);
}