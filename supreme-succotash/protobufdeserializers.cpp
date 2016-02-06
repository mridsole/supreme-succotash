#include "stdafx.h"

#include "protobufdeserializers.h"

using namespace deserialize;

bool BaseNetworkable::deserialize(Stream& stream) {

	Deserializable::deserialize(stream);

	if (!inRange(stream)) { fail(); return false; }

	// pretty simple
	advance(stream, 1);
	this->uid = readVarint32(stream);
	if (!inRange(stream)) { fail(); return false; }
	advance(stream, 1);
	this->group = readVarint32(stream);
	if (!inRange(stream)) { fail(); return false; }
	advance(stream, 1);
	this->prefabID = readVarint32(stream);
	if (!inRange(stream)) { fail(); return false; }

	success(); return true;
}

bool BaseNetworkable::deserializeLengthDelimited(Stream& stream) {

	Deserializable::deserializeLengthDelimited(stream);

	if (!inRange(stream)) { fail(); return false; }

	// yer na
	uint32_t len = readVarint32(stream);
	this->deserialize(stream);

	success(); return true;
}

bool BaseEntity::deserializeLengthDelimited(Stream& stream) {

	Deserializable::deserializeLengthDelimited(stream);

	if (!inRange(stream)) { fail(); return false; }

	uint32_t len = readVarint32(stream);
	uint8_t const * start = stream.bytes;

	while (stream.bytes - start < len) {

		if (!inRange(stream)) { fail(); return false; }

		uint32_t type = readByte(stream);

		if (type == 10) {
			this->pos = readVector3f(stream);

		} else if (type == 18) {
			this->rot = readVector3f(stream);

		} else if (type == 24) {
			this->flags = readVarint64(stream);

		} else if (type == 32) {

			this->skin = readVarint64(stream);
		}
	}

	success(); return true;
}

// DON'T USE THIS YET
bool BasePlayer::deserialize(Stream& stream) {

	Deserializable::deserialize(stream);

	// skipping a lot of things here
	while (true) {

		if (!inRange(stream)) { fail(); return false; }

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

	success(); return true;
}

// .. use this instead
bool BasePlayer::deserializeLengthDelimited(Stream& stream) {

	Deserializable::deserializeLengthDelimited(stream);

	if (!inRange(stream)) { fail(); return false; }

	// read the length
	uint32_t len = readVarint32(stream);
	uint8_t const * start = stream.bytes;

	while (stream.bytes - start < len) {

		if (!inRange(stream)) { fail(); return false; }

		uint32_t type = readVarint32(stream);

		// stuff we want
		if (type == (uint32_t)10) {
			readString(stream, this->name);
		} else if (type == (uint32_t)16) {
			this->userid = readVarint64(stream);

			// stuff we read just to get past:
		} else if (type == (uint32_t)56) {
			this->playerFlags = readVarint64(stream);
		} else if (type == (uint32_t)64) {
			this->heldEntity = readVarint32(stream);
		} else if (type == (uint32_t)77) {
			this->health = readFloat(stream);

			// more stuff to get past:
		} else if (type == (uint32_t)125) {
			this->skinCol = readFloat(stream);
		} else if (type == (uint32_t)133) {
			this->skinTex = readFloat(stream);
		} else if (type == (uint32_t)141) {
			this->skinMesh = readFloat(stream);
		
			// finally, skip everything else:
		} else {

			advance(stream, readVarint32(stream));
		}
	}

	success(); return true;
}


// deserialize an Entities message
bool Entity::deserialize(Stream& stream) {

	Deserializable::deserialize(stream);

	// for now we only read the BaseNetworkable and the BasePlayer
	// but there is some other useful info in here
	while (true) {

		if (!inRange(stream)) { fail(); return false; }

		int32_t entType = readVarint32(stream);

		if (entType == (int32_t)10) {
			if (!this->baseNetworkable.deserializeLengthDelimited(stream)) {
				fail(); return false;
			}

		} else if (entType == (int32_t)18) {
			if (!this->baseEntity.deserializeLengthDelimited(stream)) {
				fail(); return false;
			}

		} else if (entType == (int32_t)26) {
			if (!this->basePlayer.deserializeLengthDelimited(stream)) {
				fail(); return false;
			}

		} else if (entType != (int32_t)800) {

			// skip this
			advance(stream, readVarint32(stream));

		} else {

			if (!inRange(stream)) { fail(); return false; }

			// reached the end of the entity
			this->createdThisFrame = (bool)readByte(stream);

			success(); return true;
		}
	}
}

bool InputState::deserializeLengthDelimited(Stream& stream) {

	Deserializable::deserializeLengthDelimited(stream);

	if (!inRange(stream)) { fail(); return false; }

	uint32_t len = readVarint32(stream);
	uint8_t const * start = stream.bytes;

	while (stream.bytes - start < len) {

		if (!inRange(stream)) { fail(); return false; }

		int32_t type = readByte(stream);

		if (type == 8) {
			this->buttons = readVarint64(stream);
			
		} else if (type == 18) {
			this->aimAngles = readVector3f(stream);

		}
	}
	
	success(); return true;
}

bool ModelState::deserializeLengthDelimited(Stream& stream) {

	Deserializable::deserializeLengthDelimited(stream);

	if (!inRange(stream)) { fail(); return false; }

	uint32_t len = readVarint32(stream);
	uint8_t const * start = stream.bytes;

	while (stream.bytes - start < len) {

		if (!inRange(stream)) { fail(); return false; }

		int32_t type = readByte(stream);

		if (type == 37) {
			this->waterLevel = readFloat(stream);

		} else if (type == 61) {
			this->aiming = readFloat(stream);

		} else if (type == 82) {
			this->lookDir = readVector3f(stream);
			
		} else if (type == 88) {
			this->flags = readVarint64(stream);

		}
	}

	success(); return true;
}


bool PlayerTick::deserialize(Stream& stream) {

	Deserializable::deserialize(stream);

	while (true) {

		if (!inRange(stream)) { fail(); return false; }

		int32_t type = readByte(stream);

		if (type == (int32_t)10) {
			if (!this->inputState.deserializeLengthDelimited(stream)) {
				fail(); return false;
			}

		} else if (type == (int32_t)18) {
			this->position = readVector3f(stream);

		} else if (type == (int32_t)34) {
			if (!this->modelState.deserializeLengthDelimited(stream)) {
				fail(); return false;
			}

		} else if (type == (int32_t)40) {
			this->activeItem = readVarint32(stream);

		} else if (type == (int32_t)50) {
			this->eyePos = readVector3f(stream);

		} else if (type == -1) {
			
			success(); return true;
		}
	}
}