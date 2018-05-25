
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include "Server.h"
#include "PacketType.h"
#include <iostream>


/*
*	Constructor to set up the connection socket
*/

Server::Server(int port)
{
	m_listner.listen(port);
	m_selector.add(m_listner);
	canPlays.resize(2);
	hits.resize(2);
	coordHits.resize(2);
	turns.resize(2);
}

/*
 *	this function in charge of received new client and if 2 clients are connected
 *	they can send and received messages
*/

void Server::run()
{
	while (true)
	{
		//Handle new coming clients
		if (m_selector.wait())
		{
			if (m_selector.isReady(m_listner))
			{
				std::shared_ptr<sf::TcpSocket> Client = std::make_shared<sf::TcpSocket>();
				if (m_listner.accept(*Client) == sf::Socket::Done)
				{
					m_clients.emplace_back(std::move(std::make_pair(Client, "")));
					m_selector.add(*Client);
				}
				turns.at(0) = true;
			}
			if (m_clients.size() == ACCEPT_CLIENTS)
			{
				handlePackets();
			}
		}
	}
}
/*
*	this function in charge of received and send messages between 2 clients
*/

void Server::handlePackets()
{
	sf::Socket::Status status;
	std::string msg;
	PacketType MsgType;
	sf::Packet packet;
	sf::Vector2i vec;
	std::string sign;
	char tmp[1];
	bool canPlay;
	bool hit;
	int score;
	tmp[0] = '\0';
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
			if (m_selector.wait())
			{
				sf::TcpSocket& client = *(m_clients.at(i).first);
				if (m_selector.isReady(client))
				{
					status = client.receive(packet);
					switch (status)
					{
					case sf::Socket::Done:
						packet >> MsgType;
						if (MsgType == INITIAL_NAME_DATA)
						{
							packet >> msg;
							m_clients.at(i).second = msg;
							std::cout << msg + " has joined\n";
							broadCast(INITIAL_NAME_DATA, msg, !i);
						}
						if (MsgType == INITIAL_DATA)
						{
							packet >> vec.x >> vec.y >> sign;
							std::strcpy(tmp, sign.c_str());
							map[i].at(vec.x).at(vec.y) = tmp[0];
						}
						else if (MsgType == COORDINATE)
						{
							packet >> vec.x >> vec.y;
							broadCast(COORDINATE, vec, !i);
						}
						else if (MsgType == CAN_PLAY)
						{
							packet >> canPlay;
							canPlays.at(i) = canPlay;
							broadCast(CAN_PLAY, canPlays.at(i), !i);

						}
						else if (MsgType == TURN)
						{
							broadCast(TURN, turns.at(i), i);
						}
						else if (MsgType == HIT)
						{
							packet >> hit;
							hits.at(i) = hit;
						}
						else if (MsgType == GET_HIT)
						{
							packet >> vec.x >> vec.y;
							hit = getHit(!i, vec.x, vec.y);
							broadCast(GET_HIT, hit, i);
						}
						else if (MsgType == GENERAL_MSG)
						{
							packet >> msg;
							broadCast(GENERAL_MSG, msg, !i);
						}
						else if (MsgType == SCORE)
						{
							packet >> score;
							//broadCast(SCORE, score, !i);
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
}

/*
*	this function in charge of send player score
*/

void Server::broadCast(PacketType MsgType, const int score, int index)
{
	sf::Packet pack;
	pack << MsgType << score;
	m_clients.at(index).first->send(pack);
}

/*
*	this function in charge of send vector of coordinates
*/

void Server::broadCast(PacketType MsgType, const sf::Vector2i& vec, int index)
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