#include "stdafx.h"
#include "EntitiesPacketHandler.h"

#include <algorithm>

#include "packetutils.h"
#include "protobufutils.h"

#define MAX_PLAYER_NAME_LEN 512


EntitiesPacketHandler::EntitiesPacketHandler()
{
}


EntitiesPacketHandler::~EntitiesPacketHandler()
{
}


void EntitiesPacketHandler::handlePacket(uint8_t* param, const pcap_pkthdr* header,
	const uint8_t* pkt_data) {

	if (!isEntitiesPacket(param, header, pkt_data))
		return;

	// stream object for this packet
	Stream stream = Stream(pkt_data, header->len);

	// search for the bytes that seem to prefix the player name - 0x02 0x0a
	const uint8_t playerPrefix[] = { 0x02, 0x0a };
	uint8_t const* playerPrefixStart = std::search(pkt_data, pkt_data + header->len,
		&(playerPrefix[0]), &(*(playerPrefix + 2)));

	// if we didn't find it just quit now
	if (playerPrefixStart == pkt_data + header->len)
		return;

	// otherwise read the length of the player's name
	uint8_t const ** stream = &playerPrefixStart;
	//uint32_t playerNameLength = readVarint32(stream);

	// now stream is at first byte of name so get it
	char playerName[512];
	//std::copy(*stream, *stream + playerNameLength, playerName);
}


bool EntitiesPacketHandler::isEntitiesPacket(uint8_t* param, const pcap_pkthdr* header,
	const uint8_t* pkt_data) {

	// simple check for now
	if (pkt_data[69] == 0x91)
		return true;

	return false;
}

