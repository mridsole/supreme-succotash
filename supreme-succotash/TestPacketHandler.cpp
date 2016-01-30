#include "stdafx.h"
#include "TestPacketHandler.h"

#include "stdio.h"

TestPacketHandler::TestPacketHandler()
{
}


TestPacketHandler::~TestPacketHandler()
{
}

void TestPacketHandler::handlePacket(uint8_t* param, const pcap_pkthdr* header,
	const uint8_t* pkt_data) {

	printf("received packet! Length: %i\n", header->len);
}