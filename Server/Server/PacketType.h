#pragma once

#include "SFML/System.hpp"

/*
 *	PacketType are the types of messages that the server need to received and send 
*/

using PacketType = sf::Uint8;
const PacketType INITIAL_NAME_DATA = 0;
const PacketType GENERAL_MSG = 1;
const PacketType SERVER_MSG = 2;
const PacketType COORDINATE = 3;
const PacketType HIT = 4;
const PacketType GET_HIT = 5;
const PacketType SCORE = 6;
const PacketType TURN = 7;
const PacketType CAN_PLAY = 8;
const PacketType INITIAL_DATA = 9;
const PacketType GET_COORD = 10;


// limited client (only 2 clients can play in one game show)
const int ACCEPT_CLIENTS = 2;

const sf::Time TIME_OUT_TIME = sf::seconds(30);