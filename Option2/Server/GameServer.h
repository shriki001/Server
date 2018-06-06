#pragma once
#pragma warning(disable : 4996)
#include <vector>
#include <array>
#include <windows.h>
#include "SFML/Network.hpp"
#include "PacketType.h"
#include <memory>
#include <iostream>
#include <ctime>

class GameServer
{
public:
	GameServer(int port); // constructor for creating the server
	~GameServer() = default;

	void resetAll();
	bool getHit(std::array<std::array<std::array<char, ROW>, COL>, ACCEPT_CLIENTS> map,const int player, const int i, const int j) const;
	void handlePrint(std::vector <std::pair<std::shared_ptr<sf::TcpSocket>, std::string>>);
	void handleDisconnect(std::vector <std::pair<std::shared_ptr<sf::TcpSocket>, std::string>> clients, std::array<short, ACCEPT_CLIENTS> won);
	void printTime();
	void run(); // function that make the server run always

private:
	time_t rawtime;
	struct tm * timeinfo;
	sf::Clock clock;
 	std::vector<bool> turns;

	// vector of clients (pair whit client and his name)
	std::vector <std::pair<std::shared_ptr<sf::TcpSocket>, std::string>> m_clients;
	sf::TcpListener m_listner; // socket listener
	sf::SocketSelector m_selector;
	void handlePackets(std::vector <std::pair<std::shared_ptr<sf::TcpSocket>, std::string>>, sf::SocketSelector,std::vector<bool>); // function that handle sending and receiving
	// function that send message
	void broadCast(PacketType, const std::string&, int, std::vector <std::pair<std::shared_ptr<sf::TcpSocket>, std::string>>);
	// function that send player score
	void broadCast(PacketType, const short, int, std::vector <std::pair<std::shared_ptr<sf::TcpSocket>, std::string>>);
	// function that send coordinate
	void broadCast(PacketType, const sf::Vector2i&, int, std::vector <std::pair<std::shared_ptr<sf::TcpSocket>, std::string>>);
	// function that send if there are hit in the coordinate
	void broadCast(PacketType, const bool, int, std::vector <std::pair<std::shared_ptr<sf::TcpSocket>, std::string>>);
	sf::Clock ServerClock;
};