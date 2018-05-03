#pragma once
#include "SFML/Network.hpp"
#include "PacketType.h"

class Client
{
public:

	Client(const std::string & name);
	~Client() = default;

	sf::Socket::Status connect(const sf::IpAddress & IP, int port);
	sf::Socket::Status send(PacketType type, sf::Vector2i vec);
	sf::Socket::Status send(PacketType type, bool ifHit);
	sf::Socket::Status send(PacketType type, const std::string & msg);
	void receive();
	bool dis;

private:
	//PacketType m_MyType;
	std::string name;
	sf::TcpSocket me;
	
};

