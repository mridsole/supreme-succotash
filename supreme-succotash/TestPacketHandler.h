#pragma once

#include "PacketHandler.h"

class TestPacketHandler : public PacketHandler
{

public:

	TestPacketHandler();
	virtual ~TestPacketHandler();

	virtual void handlePacket(uint8_t* param, const pcap_pkthdr* header,
		const uint8_t* pkt_data);
};

