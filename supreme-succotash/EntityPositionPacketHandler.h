#pragma once
#include "PacketHandler.h"

#include "stdint.h"
#include <map>
#include <string>

#include "SFML/Graphics.hpp"

#include "packetutils.h"

class EntityPositionPacketHandler : public PacketHandler
{

public:

	// representation of entity
	struct Entity {

		uint16_t id;
		sf::Vector3f position;
		sf::Vector3f rotation;

		bool hasName;
		std::string name;
		std::string displayStr;

		// is it zuckles?
		bool isZuckles;

		unsigned int packetsSinceObserved;

		Entity(uint16_t _id, float x, float y, float z) :
			id(_id),
			position(x, y, z),
			hasName(false),
			name(),
			isZuckles(false),
			packetsSinceObserved(0)
		{}

		// shouldn't be used but throws errors otherwise?
		Entity() {};
	};

	using EntityMap = std::map<uint32_t, Entity>;

	EntityPositionPacketHandler(EntityMap* _entities);
	~EntityPositionPacketHandler();

	void handlePacket(uint8_t* param, const pcap_pkthdr* header,
		const uint8_t* pkt_data);

	// Static functions for dealing with packets
	static bool IsEntityPositionPacket(const uint8_t* pkt_data, uint32_t len);

private:

	unsigned int pruneEntities();

	// map all observed entities by their ID
	EntityMap* entities;

	// every this many packets, remove entities that haven't been observed for a while
	unsigned int howOftenToPrune;

	// how many entity position update packets must happen where a given entity
	// has not been observed for it to be pruned
	unsigned int pruneAfter;
};