#pragma once

#include "PacketHandler.h"

#include "stdint.h"

#include "SFML/Graphics.hpp"

#include "packetutils.h"

class TickPacketHandler : public PacketHandler
{

public:

	TickPacketHandler();
	~TickPacketHandler();

	void handlePacket(uint8_t* param, const pcap_pkthdr* header,
		const uint8_t* pkt_data);

	// expose the last player state
	inline const sf::Vector3f getLastPlayerPosition() const {
		return this->lastPlayerPosition;
	}

	inline const sf::Vector3f getLastPlayerRotation() const {
		return this->lastPlayerRotation;
	}

	// static functions for deciphering the packet data

	static bool IsTickPacket(const uint8_t* pkt_data, uint32_t len);

private:

	// the last position and rotation of the player
	sf::Vector3f lastPlayerPosition;
	sf::Vector3f lastPlayerRotation;
};

