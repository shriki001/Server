#include "Client.h"
#include "PacketType.h"
#include <iostream>

Client::Client(const std::string & name) :name(name)/*, m_MyType(P1)*/ {}


sf::Socket::Status Client::connect(const sf::IpAddress & IP, int port)
{
	//connect to server
	sf::Socket::Status stat = me.connect(IP, port);
	me.setBlocking(false);
	return stat;
}
sf::Socket::Status Client::send(PacketType type, const std::string & msg)
{
	sf::Packet packet;
	packet  << type << msg;
	return me.send(packet);
}
sf::Socket::Status Client::send(PacketType type, sf::Vector2i vec)
{
	sf::Packet packet;
	packet  << type << vec.x << vec.y;
	return me.send(packet);
}

sf::Socket::Status Client::send(PacketType type, bool ifHit)
{
	sf::Packet packet;
	packet  << type << ifHit;
	return me.send(packet);
}

void Client::receive()
{
	bool hit;
	std::string msg;
	sf::Vector2i vec;
	sf::Packet packet;
	PacketType type;
//	PacketType ClientType;
	sf::Socket::Status status = me.receive(packet);
	switch (status)
	{
	case sf::Socket::Done:
		//packet >> ClientType;
// 		if (ClientType == m_MyType)
// 		{
			packet >> type;
			if (type == INITIAL_NAME_DATA)
			{
				packet >> msg;
				std::cout << "My Partner Name is: " << msg << "\n";
			}
			else if (type == SERVER_MSG)
			{
				packet >> msg;
				std::cout << "Message From Server: " << msg << "\n";
			}
			else if (type == COORDINATE)
			{
				packet >> vec.x >> vec.y;
				std::cout << "Coordinates: " << vec.x << " , " << vec.y << "\n";
			}
			else if (type == HIT_OR_NOT)
			{
				packet >> hit;
				(hit == 0) ? msg = "false" : msg = true;
				std::cout << "Hit Or Not: " << msg << "\n";
			}
			else if (type == SERVER_MSG)
			{
				packet >> msg;
				std::cout << msg << "\n";
				me.disconnect();
			}
		//}
		break;
	}
}