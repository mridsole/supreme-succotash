#include "protobufdeserializers.h"

using namespace deserialize;

void BaseNetworkable::deserialize(Stream& stream) {

	// pretty simple
	advance(stream, 1);
	this->uid = readVarint32(stream);
	advance(stream, 1);
	this->group = readVarint32(stream);
	advance(stream, 1);
	this->prefabID = readVarint32(stream);
}


// DON'T USE THIS YET
void BasePlayer::deserialize(Stream& stream) {

	// skipping a lot of things here
	while (true) {

		uint8_t type = readByte(stream);

		if (type == (uint8_t)10) {
			readString(stream, this->name);

		} else if (type == (uint8_t)16) {
			this->userid = readVarint64(stream);

		} else if (type == (uint8_t)56) {
			this->playerFlags = readVarint64(stream);
		} else if (type == (uint8_t)64) {
			this->heldEntity = readVarint32(stream);
		} else if (type == (uint8_t)77) {
			this->health = readFloat(stream);

		}
	}
}

// .. use this instead
void BasePlayer::deserializeLengthDelimited(Stream& stream) {

	// read the length
	uint32_t len = readVarint32(stream);

	const uint8_t* start = stream.bytes;

	while (stream.bytes - start < len) {

		uint8_t type = readByte(stream);

		if (type == (uint8_t)10) {
			readString(stream, this->name);

		} else if (type == (uint8_t)16) {
			this->userid = readVarint64(stream);

		} else if (type == (uint8_t)56) {
			this->playerFlags = readVarint64(stream);
		} else if (type == (uint8_t)64) {
			this->heldEntity = readVarint32(stream);
		} else if (type == (uint8_t)77) {
			this->health = readFloat(stream);

		}
	}
}


// deserialize an Entities message
void Entity::deserialize(Stream& stream) {

	// for now we only read the BaseNetworkable and the BasePlayer
	// but there is some other useful info in here
	while (true) {

		uint8_t entType = readByte(stream);

		if (entType == (uint8_t)10) {
			baseNetworkable.deserialize(stream);

		} else if (entType == (uint8_t)26) {
			basePlayer.deserialize(stream);

		} else if (entType != (uint8_t)160) {
			skipEntityComponent(stream, entType);

		} else {

			// reached the end of the entity
			advance(stream, 1);
			createdThisFrame = (bool)readByte(stream);

			return;
		}
	}
}

void Entity::skipEntityComponent(Stream& stream, uint8_t entType) {

	if (entType < (uint8_t)128) {

		// read length and skip
		advance(stream, readVarint32(stream));
	} else {

		// advance 1 then read length and skip
		advance(stream, 1);
		advance(stream, readVarint32(stream));
	}
}