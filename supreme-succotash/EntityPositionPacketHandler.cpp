#include "stdafx.h"
#include "EntityPositionPacketHandler.h"

#include "protobufutils.h"

#include "stdio.h"

EntityPositionPacketHandler::EntityPositionPacketHandler(EntityMap* _entities):
	entities(_entities),
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

	// stream for our packet
	Stream stream = Stream(pkt_data, header->len);
	advance(stream, 66);

	uint32_t entityID = readUInt32(stream);

	// read the position
	float x = readFloat(stream);
	float y = readFloat(stream);
	float z = readFloat(stream);

	// read the rotation
	float pitch = readFloat(stream);
	float yaw	= readFloat(stream);
	float roll	= readFloat(stream);

	// if the given id doesn't exist make an entry for it and store the pos
	// otherwise update the existing pos
	if (entities->find(entityID) == entities->end()) {

		auto pair = std::pair<uint32_t, Entity>(entityID, Entity(entityID, x, y, z));
		pair.second.rotation = sf::Vector3f(pitch, yaw, roll);
		entities->insert(pair);

	} else {

		(*entities)[entityID].position.x = x;
		(*entities)[entityID].position.y = y;
		(*entities)[entityID].position.z = z;

		(*entities)[entityID].rotation.x = pitch;
		(*entities)[entityID].rotation.y = yaw;
		(*entities)[entityID].rotation.z = roll;

		// set prune counter to zero
		(*entities)[entityID].packetsSinceObserved = 0;
	}

	// increment prune counter for all entities 
	for (auto& pair : *entities) {

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

	if (len >= 94 && pkt_data[65] == 0x96)
		return true;

	return false;
}

// stop tracking entities we haven't observed for a while
unsigned int EntityPositionPacketHandler::pruneEntities() {

	unsigned int entitiesPruned = 0;

	EntityMap::iterator it = entities->begin();

	while (it != entities->end()) {

		if (it->second.packetsSinceObserved >= pruneAfter) {

			EntityMap::iterator it_next;
			it = entities->erase(it);
			entitiesPruned++;

		} else {

			it++;
		}
	}

	return entitiesPruned;
}