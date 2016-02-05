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

	// NOTE: the 0a is the start of a base networkable - the 0c is the length
	// of the base networkable, the 08 is the first byte written by the base networkable
	constexpr uint8_t searchFor1[] = { 0x0a, 0x0c, 0x08 };
	constexpr uint8_t searchFor2[] = { 0x0a, 0x0d, 0x08 };

	while (stream.bytes - stream.bytesStart < (int)stream.len - 4) {

		// reset the deserializer
		entityDeserializer.reset();

		// advance to the next entity
		uint8_t const * nextEntity1 = std::search(stream.bytes, stream.bytesEnd,
			searchFor1, searchFor1 + 3);
		uint8_t const * nextEntity2 = std::search(stream.bytes, stream.bytesEnd,
			searchFor2, searchFor2 + 3);

		if (nextEntity1 == stream.bytesEnd &&
			nextEntity2 == stream.bytesEnd) break;

		// advance to the closest next entity
		if (nextEntity1 - stream.bytes < nextEntity2 - stream.bytes)
			advance(stream, nextEntity1 - stream.bytes);
		else
			advance(stream, nextEntity2 - stream.bytes);

		// deserialize it
		entityDeserializer.deserialize(stream);

		// if we got a BasePlayer from it, print out the stats
		if (entityDeserializer.baseNetworkable.hasSerialized() &&
			entityDeserializer.basePlayer.hasSerialized()) {


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


bool EntitiesPacketHandler::isEntitiesPacket(uint8_t* param, const pcap_pkthdr* header,
	const uint8_t* pkt_data) {

	// simple check for now
	if (pkt_data[69] == 0x91)
		return true;

	return false;
}
