#pragma once

#include "SFML/System.hpp"

/*
 *	PacketType are the types of messages 
 *	that the server need to received and send to Game
*/

using PacketType = sf::Uint8;
const PacketType INITIAL_NAME_DATA = 0;
const PacketType WHOWON = 1;
const PacketType SERVER_MSG = 2;
const PacketType COORDINATE = 3;
const PacketType HIT = 4;
const PacketType GET_HIT = 5;
const PacketType SCORE = 6;
const PacketType TURN = 7;
const PacketType CAN_PLAY = 8;
const PacketType INITIAL_DATA = 9;
const PacketType GET_COORD = 10;

/*
*	PacketChat are the types of messages 
*	that the server need to received and send to Chat
*/

using PacketChat = sf::Uint8;
const PacketChat NAME = 0;
const PacketChat GMSG = 1;
const PacketChat SMSG = 2;

// limited client (only 2 clients can play in one game show)
const int ACCEPT_CLIENTS = 2;
const int WON = 2;
const int ROW = 10;
const int COL = 10;

const sf::Time TIME_OUT_TIME = sf::seconds(30);