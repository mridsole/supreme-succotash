#pragma once

#include "PacketHandler.h"

#include "stdint.h"

#include "SFML/Graphics.hpp"

#include "packetutils.h"

class TickPacketHandler : public PacketHandler
{

public:

	struct ClientState {

		sf::Vector3f position;
		sf::Vector3f rotation;

		ClientState(sf::Vector3f _position,
			sf::Vector3f _rotation)
			:
			position(_position),
			rotation(_rotation)
		{}

		ClientState() :
			position(0, 0, 0),
			rotation(0, 0, 0)
		{}
	};

	TickPacketHandler();
	~TickPacketHandler();

	void handlePacket(uint8_t* param, const pcap_pkthdr* header,
		const uint8_t* pkt_data);

	inline ClientState& getClientState() {
		return clientState;
	}

	// expose the last player state
	inline const sf::Vector3f getLastPlayerPosition() const {
		return clientState.position;
	}

	inline const sf::Vector3f getLastPlayerRotation() const {
		return clientState.rotation;
	}

	// static functions for deciphering the packet data

	static bool IsTickPacket(const uint8_t* pkt_data, uint32_t len);

private:

	// use this now instead
	ClientState clientState;
};

