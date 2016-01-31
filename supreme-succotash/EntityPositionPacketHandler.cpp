#include "stdafx.h"
#include "EntityPositionPacketHandler.h"

#include "stdio.h"

EntityPositionPacketHandler::EntityPositionPacketHandler():
	entityPositions()
{
}


EntityPositionPacketHandler::~EntityPositionPacketHandler()
{
}

void EntityPositionPacketHandler::handlePacket(uint8_t* param, const pcap_pkthdr* header,
	const uint8_t* pkt_data) {

	if (!EntityPositionPacketHandler::IsEntityPositionPacket(pkt_data, header->len)) 
		return;

	// the coordinates
	float x = *((float*)(pkt_data + 70));
	float y = *((float*)(pkt_data + 74));
	float z = *((float*)(pkt_data + 78));

	uint16_t entityID = *((uint16_t*)(pkt_data + 66));

	// if the given id doesn't exist make an entry for it and store the pos
	// otherwise update the existing pos
	if (entityPositions.find(entityID) == entityPositions.end()) {

		entityPositions.insert(std::pair<uint16_t, sf::Vector3f>(entityID, sf::Vector3f(x, y, z)));
	} else {

		entityPositions[entityID].x = x;
		entityPositions[entityID].y = y;
		entityPositions[entityID].z = z;
	}
}

bool EntityPositionPacketHandler::IsEntityPositionPacket(
	const uint8_t* pkt_data, uint32_t len) {

	if (len == 98 && pkt_data[65] == 0x96)
		return true;

	return false;
}
