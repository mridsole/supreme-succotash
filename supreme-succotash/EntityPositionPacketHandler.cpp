#include "stdafx.h"
#include "EntityPositionPacketHandler.h"

#include "stdio.h"

EntityPositionPacketHandler::EntityPositionPacketHandler():
	entities(),
	howOftenToPrune(5000),
	pruneAfter(500)
{
}


EntityPositionPacketHandler::~EntityPositionPacketHandler()
{
}

void EntityPositionPacketHandler::handlePacket(uint8_t* param, const pcap_pkthdr* header,
	const uint8_t* pkt_data) {

	// number of position update packets since last prune
	static unsigned int packetCount = 0;

	if (!EntityPositionPacketHandler::IsEntityPositionPacket(pkt_data, header->len)) 
		return;

	// the coordinates
	float x = *((float*)(pkt_data + 70));
	float y = *((float*)(pkt_data + 74));
	float z = *((float*)(pkt_data + 78));

	uint16_t entityID = *((uint16_t*)(pkt_data + 66));

	// if the given id doesn't exist make an entry for it and store the pos
	// otherwise update the existing pos
	if (entities.find(entityID) == entities.end()) {

		entities.insert( std::pair<uint16_t, Entity>(entityID, Entity(entityID, x, y, z)) );

	} else {

		entities[entityID].position.x = x;
		entities[entityID].position.y = y;
		entities[entityID].position.z = z;

		// set prune counter to zero
		entities[entityID].packetsSinceObserved = 0;
	}

	// increment prune counter for all entities 
	for (auto& pair : entities) {

		pair.second.packetsSinceObserved++;
	}

	packetCount++;

	// if we due for a prune then do it
	if (packetCount >= howOftenToPrune) {

		pruneEntities();
		packetCount = 0;
	}
}

bool EntityPositionPacketHandler::IsEntityPositionPacket(
	const uint8_t* pkt_data, uint32_t len) {

	if (len == 98 && pkt_data[65] == 0x96)
		return true;

	return false;
}

// stop tracking entities we haven't observed for a while
unsigned int EntityPositionPacketHandler::pruneEntities() {

	unsigned int entitiesPruned = 0;

	EntityMap::iterator it = entities.begin();

	while (it != entities.end()) {

		if (it->second.packetsSinceObserved >= pruneAfter) {

			EntityMap::iterator it_next;
			it = entities.erase(it);
			entitiesPruned++;

		} else {

			it++;
		}
	}

	return entitiesPruned;
}