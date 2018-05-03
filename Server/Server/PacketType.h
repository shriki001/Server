#pragma once

#include "SFML/System.hpp"

/*
 *	PacketType are the types of messages that the server need to received and send 
*/

typedef sf::Uint8 PacketType;
const PacketType INITIAL_NAME_DATA = 0;
const PacketType SERVER_MSG = 1;
const PacketType COORDINATE = 2;
const PacketType HIT_OR_NOT = 3;

// limited client (only 2 clients can play in one game show)
const int ACCEPT_CLIENTS = 2;
