#pragma once
#include <vector>
#include <array>
#include "SFML/Network.hpp"
#include "PacketType.h"
#include <memory>


class Server
{
public:
	Server(int port); // constructor for creating the server
	~Server() = default;
	bool getHit(const int player, const int i, const int j) const
	{
		try
		{
			if (i == -1 || j == -1)
				return false;
			return map[player].at(i).at(j) != '-';
		}
		catch (std::exception) { return false; }
	}

	void run(); // function that make the server run always

private:


	std::array<std::array<std::array<char, 10>, 10>, 2> map;

	std::vector<bool> canPlays;
	std::vector<sf::Vector2i> coordHits;
	std::vector<bool> turns;
	std::vector<bool> hits;
	// vector of clients (pair whit client and his name)
	std::vector <std::pair<std::shared_ptr<sf::TcpSocket>, std::string>> m_clients;
	sf::TcpListener m_listner; // socket listener
	sf::SocketSelector m_selector;
	void handlePackets(); // function that handle sending and receiving
	// function that send message
	void broadCast(PacketType, const std::string&, int);
	// function that send player score
	void broadCast(PacketType, const int, int);
	// function that send coordinate
	void broadCast(PacketType, const sf::Vector2i&, int);
	// function that send if there are hit in the coordinate
	void broadCast(PacketType, const bool, int);
};