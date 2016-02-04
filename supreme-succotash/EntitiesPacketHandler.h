#pragma once

#include "PacketHandler.h"

// use this to match player names to IDs
class EntitiesPacketHandler : public PacketHandler
{

public:

	EntitiesPacketHandler();
	~EntitiesPacketHandler();

	void handlePacket(uint8_t* param, const pcap_pkthdr* header,
		const uint8_t* pkt_data);


	static bool isEntitiesPacket(uint8_t* param, const pcap_pkthdr* header,
		const uint8_t* pkt_data);
};