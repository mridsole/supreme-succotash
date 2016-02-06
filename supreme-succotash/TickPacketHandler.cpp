#include "stdafx.h"
#include "TickPacketHandler.h"

#include <algorithm>

#include "string.h"
#include "stdio.h"

#include "protobufutils.h"
#include "protobufdeserializers.h"

TickPacketHandler::TickPacketHandler() :
	clientState()
{}

TickPacketHandler::~TickPacketHandler()
{}

void TickPacketHandler::handlePacket(uint8_t* param, const pcap_pkthdr* header,
	const uint8_t* pkt_data) {

	if (!TickPacketHandler::IsTickPacket(pkt_data, header->len)) return;

	Stream stream = Stream(pkt_data, header->len);
	advance(stream, 70);

	// deserialize it
	deserialize::PlayerTick tickDeserializer;
	tickDeserializer.deserialize(stream);

	// get the info we want
	clientState.rotation = tickDeserializer.inputState.aimAngles;
	clientState.position = tickDeserializer.position;
}


bool TickPacketHandler::IsTickPacket(const uint8_t* pkt_data, uint32_t len) {

	if ((len >= 150 && len <= 180) && pkt_data[69] == 0x9b)
		return true;

	return false;
}