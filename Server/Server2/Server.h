#pragma once
#include <vector>
#include "SFML/Network.hpp"
#include "PacketType.h"
#include <memory>


class Server
{
public:
	Server(int port); // constructor for creating the server
	~Server() = default; 
	void run(); // function that make the server run always

private:
	// vector of clients (pair whit client and his name)
	std::vector <std::pair<std::shared_ptr<sf::TcpSocket>,std::string>> m_clients;
	sf::TcpListener m_listner; // socket listener
	void handlePackets(); // function that handle sending and receiving
	// function that send message
	void broadCast(PacketType, const std::string&, int);
	// function that send coordinate
	void broadCast(PacketType,const sf::Vector2i, int);
	// function that send if there are hit in the coordinate
	void broadCast(PacketType, const bool, int);
};

