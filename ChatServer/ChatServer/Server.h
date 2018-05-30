#pragma once
#pragma warning(disable : 4996)
#include <unordered_map>
#include "SFML/Network.hpp"
#include "PacketType.h"
#include <windows.h>
#include <vector>

class Server
{
private:
	typedef std::unordered_map<sf::TcpSocket *, std::string> Clients;
	Clients clients;
	sf::TcpListener listner;
	void handlePackets();
	void broadCast(PacketType type, const std::string & msg);

public:
	Server(unsigned short port);
	~Server() = default;
	void printTime();
	time_t rawtime;
	struct tm * timeinfo;
	void run();
};