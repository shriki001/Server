#include "GameServer.h"
#include "PacketType.h"
#include <iostream>


/*
*	Constructor to set up the connection socket
*/

GameServer::GameServer(int port)
{
	m_listner.listen(port);
	resetAll();
	printTime();
	std::cout << "Game Server Start\n";
}


void GameServer::resetAll()
{
	m_selector.clear();
	m_clients.clear();
	m_selector.add(m_listner);
	turns.resize(ACCEPT_CLIENTS);
	for (int i = 0; i < ACCEPT_CLIENTS; ++i)
		turns.at(i) = false;
}


void GameServer::handlePrint(const std::string& Player1, const std::string& Player2)
{
	clock.restart();
	printTime();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
	std::cout << "Game Start\n";
	printTime();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
	std::cout << "Now Playing: " << Player1 << " V.S " << Player2 << "\n";
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 15);
}


void GameServer::handleDisconnect(const std::string& Player1, const std::string& Player2, const std::array<short, ACCEPT_CLIENTS>& won)
{
	printTime();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 11);
	if (won.at(0) == 1)
		std::cout << Player1 + " Has WON!!\n";
	else if (won.at(1) == 1)
		std::cout << Player2 + " Has WON!!\n";
	else std::cout << "NoBody has WON!!\n";

	printTime();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 10);
	std::cout << "Played Game Time: " << int(clock.getElapsedTime().asSeconds() / 60) << ":";
	(int(fmod(clock.getElapsedTime().asSeconds(), 60)) < 10) ? std::cout << "0" :
		std::cout << int(fmod(clock.getElapsedTime().asSeconds(), 60));
	std::cout << " Minuets" << "\n";
	printTime();
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 13);
	std::cout << "Game End\n";
}


void GameServer::printTime()
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

void GameServer::run()
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
				broadCast(SERVER_MSG, msg, m_clients.at(0).first);
				sf::sleep(sf::milliseconds(500));
				resetAll();
			}
			if (m_clients.size() == ACCEPT_CLIENTS)
			{
				sf::Thread playGameFor2([&]()
				{handlePackets(m_clients, m_selector, turns); });
				playGameFor2.launch();
				sf::sleep(sf::milliseconds(500));
				resetAll();
				sf::Thread runAgain([&]()
				{run(); });
				runAgain.launch();
			}
		}
	}
}

/*
*	this function in charge of received and send messages between 2 clients
*/

void GameServer::handlePackets(std::vector <std::pair<std::shared_ptr<sf::TcpSocket>, std::string>>clients, sf::SocketSelector selector, std::vector<bool>turn)
{
	std::array<std::array<std::array<char, ROW>, COL>, ACCEPT_CLIENTS> map;
	for (int num = 0; num < ACCEPT_CLIENTS; ++num)
		for (int i = 0; i < ROW; ++i)
			for (int j = 0; j < COL; ++j)
				map[num].at(j).at(i) = '-';

	std::array<bool, ACCEPT_CLIENTS> dis;
	std::array<short, ACCEPT_CLIENTS> counting;
	std::array<short, ACCEPT_CLIENTS> won;
	std::array<bool, ACCEPT_CLIENTS> canPlays;
	std::array<sf::Vector2i, ACCEPT_CLIENTS> coordHits;
	std::array<bool, ACCEPT_CLIENTS> hits;

	for (int i = 0; i < ACCEPT_CLIENTS; ++i)
	{
		coordHits.at(i) = { -1,-1 };
		counting.at(i) = 0;
		won.at(i) = 0;
		dis.at(i) = false;
	}
	////////////////////////////////////////////////////////////////////////////

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
	bool first = true;

	while (clients.size() == ACCEPT_CLIENTS)
	{
		for (int i = 0; i < clients.size(); ++i)
		{
			if (selector.wait())
			{
				sf::TcpSocket& client = *(clients.at(i).first);
				if (selector.isReady(client))
				{
					status = client.receive(packet);
					switch (status)
					{
					case sf::Socket::Done:
						packet >> MsgType;
						if (MsgType == INITIAL_NAME_DATA)
						{
							packet >> msg;
							clients.at(i).second = msg;
							broadCast(INITIAL_NAME_DATA, msg, clients.at(!i).first);
							count++;
							if (count == ACCEPT_CLIENTS)
							{
								handlePrint(clients.at(0).second, clients.at(0).second);
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
							vec = { -1,-1 };
						}
						else if (MsgType == GET_COORD)
						{
							broadCast(GET_COORD, coordHits.at(!i), clients.at(i).first);
							coordHits.at(!i) = { -1,-1 };
						}
						else if (MsgType == CAN_PLAY)
						{
							packet >> canPlay;
							canPlays.at(i) = canPlay;
							broadCast(CAN_PLAY, canPlays.at(i), clients.at(!i).first);
						}
						else if (MsgType == TURN)
							broadCast(TURN, turns.at(i), clients.at(i).first);

						else if (MsgType == HIT)
						{
							packet >> hit;
							hits.at(i) = hit;
						}
						else if (MsgType == GET_HIT)
						{
							packet >> vec.x >> vec.y;
							if (vec == sf::Vector2i(-1, -1))
								hit = false;
							else hit = (map[!i][vec.x][vec.y] != '-');
							broadCast(GET_HIT, hit, clients.at(i).first);
							vec = { -1,-1 };
							if (hit)
								counting.at(i)++;
						}
						else if (MsgType == WHOWON)
						{
							if (first && counting.at(i) == WON)
							{
								won.at(i) = 1;
								won.at(!i) = 2;
								first = false;
							}
							broadCast(WHOWON, won.at(i), clients.at(i).first);
						}
						break;
					case sf::Socket::Disconnected:
						dis.at(i) = true;
						if (won.at(0) == 0 && won.at(1) == 0)
						{
							won.at(i) = 2;
							won.at(!i) = 1;
							first = false;
						}
						broadCast(SERVER_MSG, clients.at(i).second + " has been disconnected\n", clients.at(!i).first);
						if (dis.at(0) && dis.at(1))
						{
							handleDisconnect(clients.at(0).second, clients.at(1).second, won);
							clients.clear();
						}
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

void GameServer::broadCast(PacketType MsgType, const std::string &msg, std::shared_ptr<sf::TcpSocket> client)
{
	sf::Packet pack;
	pack << MsgType << msg;
	client->send(pack);
}

/*
*	this function in charge of send player score
*/

void GameServer::broadCast(PacketType MsgType, const short won, std::shared_ptr<sf::TcpSocket> client)
{
	sf::Packet pack;
	pack << MsgType << won;
	client->send(pack);
}

/*
*	this function in charge of send vector of coordinates
*/

void GameServer::broadCast(PacketType MsgType, const sf::Vector2i& vec, std::shared_ptr<sf::TcpSocket> client)
{
	sf::Packet pack;
	pack << MsgType << vec.x << vec.y;
	client->send(pack);
}

/*
*	this function in charge of send if there are hit in the coordinate
*/

void GameServer::broadCast(PacketType MsgType, const bool hit, std::shared_ptr<sf::TcpSocket> client)
{
	sf::Packet pack;
	pack << MsgType << hit;
	client->send(pack);
}
