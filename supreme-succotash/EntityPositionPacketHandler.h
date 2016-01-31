#pragma once
#include "PacketHandler.h"

#include "stdint.h"
#include <map>

#include "SFML/Graphics.hpp"

#include "packetutils.h"

class EntityPositionPacketHandler : public PacketHandler
{

public:

	EntityPositionPacketHandler();
	~EntityPositionPacketHandler();

	void handlePacket(uint8_t* param, const pcap_pkthdr* header,
		const uint8_t* pkt_data);

	std::map<uint16_t, sf::Vector3f> getEntityPositions() {

		return entityPositions;
	}

	// Static functions for dealing with packets
	static bool IsEntityPositionPacket(const uint8_t* pkt_data, uint32_t len);

private:

	// construct map of all entities. should really do this elsewhere!
	std::map<uint16_t, sf::Vector3f> entityPositions;
};