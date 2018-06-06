#pragma once
#pragma warning(disable : 4996)
#include <unordered_map>
#include "SFML/Network.hpp"
#include "PacketType.h"
#include <memory>
#include <windows.h>
#include <vector>

class ChatServer
{
private:
	typedef std::unordered_map<std::shared_ptr<sf::TcpSocket>, std::string> Clients;
	Clients clients;
	sf::TcpListener listner;
	void handlePackets();
	void broadCast(PacketChat type, const std::string & msg);

public:
	ChatServer(unsigned short port);
	~ChatServer() = default;
	void printTime();
	time_t rawtime;
	struct tm * timeinfo;
	void run();
};