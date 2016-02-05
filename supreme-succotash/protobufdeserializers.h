#pragma once

#include "protobufutils.h"

// for sf::Vector3f
#include "SFML/Graphics.hpp"

#define MAX_NAME_LEN 512

namespace deserialize {

using Vector3f = sf::Vector3f;

// interface for deserializable protobufs
class Deserializable {

public:

	Deserializable() : serialized(false) {};
	virtual ~Deserializable() {};

	virtual void deserialize(Stream& stream) {
		serialized = true;
	};

	virtual void deserializeLengthDelimited(Stream& stream) {
		serialized = true;
	};

	bool hasSerialized() { return this->serialized; }

	virtual void reset() { serialized = false; }

private:

	bool serialized;
};

class BaseNetworkable : public Deserializable {

public:

	// DATA
	uint32_t uid;
	uint32_t group;
	uint32_t prefabID;

	BaseNetworkable() {};
	~BaseNetworkable() {};

	virtual void deserialize(Stream& stream);
	virtual void deserializeLengthDelimited(Stream& stream);

	virtual void reset() {

		Deserializable::reset();
	}
};

class BaseEntity : public Deserializable {

public:

	// DATA
	Vector3f pos;
	Vector3f rot;
	int flags;
	int skin;

	BaseEntity() {};
	~BaseEntity() {};

	virtual void deserializeLengthDelimited(Stream& stream);

	virtual void reset() {

		Deserializable::reset();
	}
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

	// same goes for these
	float skinCol;
	float skinTex;
	float skinMesh;


	BasePlayer() {};
	~BasePlayer() {};

	virtual void deserialize(Stream& stream);

	// first byte is varint32 representing length
	virtual void deserializeLengthDelimited(Stream& stream);

	virtual void reset() {

		Deserializable::reset();
	}

private:

	static void skipBasePlayerComponent(Stream& stream, uint8_t type);
};

// deserializer for Entity object
class Entity : public Deserializable
{

public:

	BaseNetworkable baseNetworkable;
	BaseEntity baseEntity;
	BasePlayer basePlayer;

	bool createdThisFrame;
	
	Entity() {};
	~Entity() {};

	virtual void deserialize(Stream& stream);

	virtual void reset() {

		Deserializable::reset();

		baseNetworkable.reset();
		baseEntity.reset();
		basePlayer.reset();
	}

private:

};


class InputState : public Deserializable {

public:

	// DATA
	int buttons;
	Vector3f aimAngles;

	InputState() {};
	~InputState() {};

	virtual void deserializeLengthDelimited(Stream& stream);

	virtual void reset() {

		Deserializable::reset();
	}
};

class ModelState : public Deserializable {

public:

	// DATA
	enum class Flag {
		Ducked = 1,
		Jumped = 2,
		OnGround = 4,
		Sleeping = 8,
		Sprinting = 16,
		OnLadder = 32,
		Flying = 64
	};

	float waterLevel;
	float aiming;
	Vector3f lookDir;
	int flags;

	ModelState() {};
	~ModelState() {};

	virtual void deserializeLengthDelimited(Stream& stream);

	virtual void reset() {

		Deserializable::reset();
	}
};

// deserializer for PlayerTick object
class PlayerTick : public Deserializable
{

public:

	// DATA
	InputState inputState;
	Vector3f position;
	ModelState modelState;
	uint32_t activeItem;
	Vector3f eyePos;

	PlayerTick() {};
	~PlayerTick() {};

	virtual void deserialize(Stream& stream);

	virtual void reset() {

		Deserializable::reset();
	}
};

};

