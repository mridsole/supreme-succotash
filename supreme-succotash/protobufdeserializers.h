#pragma once

#include "protobufutils.h"

#define MAX_NAME_LEN 512

namespace deserialize {


class BaseNetworkable : public Deserializable {

public:

	// DATA
	uint32_t uid;
	uint32_t group;
	uint32_t prefabID;

	BaseNetworkable() {};
	~BaseNetworkable() {};

	void deserialize(Stream& stream);
};

class BasePlayer : public Deserializable {

public:

	// DATA
	uint8_t name[MAX_NAME_LEN];
	uint64_t userid;

	// ... skipping some things ...
	
	// it's more convenient to read these than skip them
	uint64_t playerFlags;
	uint32_t heldEntity;
	float health;


	BasePlayer() {};
	~BasePlayer() {};

	void deserialize(Stream& stream);

	// first byte is varint32 representing length
	void deserializeLengthDelimited(Stream& stream);

private:

	static void skipBasePlayerComponent(Stream& stream, uint8_t type);
};

// deserializer for Entity object
class Entity : public Deserializable
{

public:

	BaseNetworkable baseNetworkable;
	BasePlayer basePlayer;

	bool createdThisFrame;

	Entity() {};
	~Entity() {};

	void deserialize(Stream& stream);

private:

	static void skipEntityComponent(Stream& stream, uint8_t entType);
};

};