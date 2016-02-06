#include "stdafx.h"
#include "EntitiesPacketHandler.h"

#include <algorithm>

#define MAX_PLAYER_NAME_LEN 512


EntitiesPacketHandler::EntitiesPacketHandler(EntityMap* _entityMap) :
	entityMap(_entityMap)
{}


EntitiesPacketHandler::~EntitiesPacketHandler()
{}


void EntitiesPacketHandler::handlePacket(uint8_t* param, const pcap_pkthdr* header,
	const uint8_t* pkt_data) {

	if (!isEntitiesPacket(param, header, pkt_data))
		return;

	// bit of a hacky way of doing it but basically search for 0a 0d 08,
	// which occurs at all the packets that we're interested in and identifies
	// the start of the serialized entity

	deserialize::Entity entityDeserializer;

	// advance to the start of the entity
	Stream stream = Stream(pkt_data, header->len);
	advance(stream, 74);

	int i = 0;

	while (true) {

		i++;
		if (i > 100)
			printf("looks like we're stuck!\n");

		// reset the deserializer
		entityDeserializer.reset();

		// advance to the next entity
		if (!EntitiesPacketHandler::nextEntity(stream))
			break;
		
		// deserialize it
		entityDeserializer.deserialize(stream);

		// if we got a BasePlayer from it, print out the stats
		if (entityDeserializer.baseNetworkable.wasSuccessful() &&
			entityDeserializer.basePlayer.wasSuccessful() &&
			entityDeserializer.wasSuccessful()) {

			updateEntity(entityDeserializer);
		}
	}
}

void EntitiesPacketHandler::updateEntity(const deserialize::Entity& entityDeserializer) {

	uint32_t uid = entityDeserializer.baseNetworkable.uid;
	uint8_t const * name = entityDeserializer.basePlayer.name;

	// find or create the entity in the map
	// TODO: if we're creating here we need to give it it's position
	// which we currently don't deserialize

	EntityMap::iterator it = entityMap->find(uid);

	// found something
	if (it != entityMap->end()) {

		Entity& ent = it->second;

		ent.name.assign((char const *)name);
		ent.hasName = true;

	} else {

		float x = entityDeserializer.baseEntity.pos.x;
		float y = entityDeserializer.baseEntity.pos.y;
		float z = entityDeserializer.baseEntity.pos.z;

		// create something
		auto pair = std::pair<uint32_t, Entity>(uid, Entity(uid, x, y, z));
		pair.second.rotation = entityDeserializer.baseEntity.rot;
		
		pair.second.hasName = true;
		pair.second.name.assign((char const *)name);

		entityMap->insert(pair);
	}
}


bool EntitiesPacketHandler::nextEntity(Stream& stream) {

	while (stream.bytes - stream.bytesStart < stream.len - 4) {

		if (stream.bytes[0] == 0x0a && stream.bytes[2] == 0x08)
			return true;

		advance(stream, 1);
	}

	return false;
}

bool EntitiesPacketHandler::isEntitiesPacket(uint8_t* param, const pcap_pkthdr* header,
	const uint8_t* pkt_data) {

	// simple check for now
	if (pkt_data[69] == 0x91)
		return true;

	return false;
}
