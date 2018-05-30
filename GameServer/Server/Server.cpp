#include "Server.h"
#include "PacketType.h"
#include <iostream>


/*
*	Constructor to set up the connection socket
*/

Server::Server(int port)
{
	m_listner.listen(port);
	resetAll();
	printTime();
	std::cout << "Game Server Start\n";
}


void Server::resetAll()
{
	m_selector.clear();
	m_clients.clear();
	m_selector.add(m_listner);
	canPlays.resize(ACCEPT_CLIENTS);
	won.resize(ACCEPT_CLIENTS);
	counting.resize(ACCEPT_CLIENTS);
	hits.resize(ACCEPT_CLIENTS);
	turns.resize(ACCEPT_CLIENTS);
	coordHits.resize(ACCEPT_CLIENTS);
	for (int i = 0; i < ACCEPT_CLIENTS; ++i)
		coordHits.at(i) = { -1,-1 };
}


bool Server::getHit(const int player, const int i, const int j) const
{
	try
	{
		if (i == -1 || j == -1)
			return false;
		return map[player].at(i).at(j) != '-';
	}
	catch (std::exception) { return false; }
}

void Server::handlePrint()
{
	clock.restart();
	printTime();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
	std::cout << "Game Start\n";
	printTime();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
	std::cout << "Now Playing: " << m_clients.at(0).second << " V.S " << m_clients.at(1).second << "\n";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}


void Server::handleDisconnect(int i)
{
	printTime();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
	(won.at(i) == 1) ? std::cout << m_clients.at(i).second + "Has WON!!\n" :
		std::cout << m_clients.at(!i).second + "Has WON!!\n";

	printTime();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
	std::cout << "Played Game Time: " << int(clock.getElapsedTime().asSeconds() / 60) << ":";
	(int(fmod(clock.getElapsedTime().asSeconds(), 60)) < 10) ? std::cout << "0" :
		std::cout << int(fmod(clock.getElapsedTime().asSeconds(), 60));
	std::cout << " Minuets" << "\n";
	printTime();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
	std::cout << "Game End\n";
	broadCast(SERVER_MSG, m_clients.at(i).second + " has been disconnected\n", !i);
	broadCast(SERVER_MSG, m_clients.at(i).second + " has been disconnected\n", i);
	sf::sleep(sf::milliseconds(700));
	m_clients.clear();
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
/*
 *	this function in charge of received new client and if 2 clients are connected
 *	they can send and received messages
*/

void Server::run()
{
	static sf::Time elapsed = sf::Time::Zero;
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
					ServerClock.restart();
					m_clients.emplace_back(std::move(std::make_pair(Client, "")));
					m_selector.add(*Client);
				}
				turns.at(0) = true;
			}
			if (m_clients.size() == 1 && ServerClock.getElapsedTime() > TIME_OUT_TIME)
			{
				std::string msg = "Match not found!!\nTry again later\nDisconnect...";
				broadCast(SERVER_MSG, msg, 0);
				sf::sleep(sf::milliseconds(500));
				resetAll();
			}
			if (m_clients.size() == ACCEPT_CLIENTS)
			{
				handlePackets();
				resetAll();
				clearMap();
			}
		}
	}
}


void Server::clearMap()
{
	for (int num = 0; num < ACCEPT_CLIENTS; ++num)
		for (int i = 0; i < ROW; ++i)
			for (int j = 0; j < COL; ++j)
				map[num].at(j).at(i) = '-';
}
/*
*	this function in charge of received and send messages between 2 clients
*/

void Server::handlePackets()
{
	int count = 0;
	sf::Socket::Status status;
	std::string msg;
	PacketType MsgType;
	sf::Packet packet;
	sf::Vector2i vec{ -1,-1 };
	std::string sign;
	char tmp[ACCEPT_CLIENTS];
	bool canPlay;
	bool hit;

	while (m_clients.size() == ACCEPT_CLIENTS)
	{
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
							broadCast(INITIAL_NAME_DATA, msg, !i);
							count++;
							if (count == ACCEPT_CLIENTS)
							{
								handlePrint();
								count = 0;
							}
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
							coordHits.at(i) = vec;
						}
						else if (MsgType == GET_COORD)
						{
							broadCast(GET_COORD, coordHits.at(!i), i);
							coordHits.at(!i) = { -1,-1 };
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
							if (hit)
								counting.at(i)++;
						}
						else if (MsgType == WHOWON)
						{
							if (counting.at(i) == WON)
							{
								won.at(i) == 1;
								won.at(!i) == 2;
							}
							broadCast(WHOWON, won.at(i), i);
						}
						break;
					case sf::Socket::Disconnected:
						handleDisconnect(i);
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

void Server::broadCast(PacketType MsgType, const int won, int index)
{
	sf::Packet pack;
	pack << MsgType << won;
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