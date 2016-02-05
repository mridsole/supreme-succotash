#pragma once

#include "PacketHandler.h"

#include "EntityPositionPacketHandler.h"

#include "packetutils.h"
#include "protobufutils.h"

#include "protobufdeserializers.h"

// use this to match player names to IDs
class EntitiesPacketHandler : public PacketHandler
{

public:

	using Entity = EntityPositionPacketHandler::Entity;
	using EntityMap = EntityPositionPacketHandler::EntityMap;

	EntitiesPacketHandler(EntityMap* _entityMap);
	~EntitiesPacketHandler();

	void handlePacket(uint8_t* param, const pcap_pkthdr* header,
		const uint8_t* pkt_data);

	void updateEntity(const deserialize::Entity& entityDeserializer);


	static bool isEntitiesPacket(uint8_t* param, const pcap_pkthdr* header,
		const uint8_t* pkt_data);

private:

	EntityMap* entityMap;
};