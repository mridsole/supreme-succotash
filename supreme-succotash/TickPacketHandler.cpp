#include "stdafx.h"
#include "TickPacketHandler.h"

#include <algorithm>

#include "string.h"
#include "stdio.h"

TickPacketHandler::TickPacketHandler() :
	clientState()
{}

TickPacketHandler::~TickPacketHandler()
{}

void TickPacketHandler::handlePacket(uint8_t* param, const pcap_pkthdr* header,
	const uint8_t* pkt_data) {

	if (!TickPacketHandler::IsTickPacket(pkt_data, header->len)) return;

	// search string for vec3 type
	const uint8_t vec3_bytes[] = { 0x12, 0x0f, 0x0d };
	const uint8_t* vec3_search_str = &(vec3_bytes[0]);

	auto vec3_1_start = std::search(pkt_data, pkt_data + header->len,
		vec3_search_str, vec3_search_str + 3);

	float x = *((float*)(vec3_1_start + 3));
	float y = *((float*)(vec3_1_start + 8));
	float z = *((float*)(vec3_1_start + 13));

	clientState.rotation.x = x;
	clientState.rotation.y = y;
	clientState.rotation.z = z;

	// find the second vec3
	auto vec3_2_start = std::search(vec3_1_start + 3, pkt_data + header->len,
		vec3_search_str, vec3_search_str + 3);

	x = *((float*)(vec3_2_start + 3));
	y = *((float*)(vec3_2_start + 8));
	z = *((float*)(vec3_2_start + 13));

	clientState.position.x = x;
	clientState.position.y = y;
	clientState.position.z = z;
}


bool TickPacketHandler::IsTickPacket(const uint8_t* pkt_data, uint32_t len) {

	if ((len >= 162 && len <= 180) && pkt_data[69] == 0x9b)
		return true;

	return false;
}