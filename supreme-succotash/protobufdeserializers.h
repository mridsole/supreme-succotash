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

	Deserializable() : serialized(false), successful(false) {};
	virtual ~Deserializable() {};

	virtual bool deserialize(Stream& stream) {
		serialized = true;
		return serialized;
	};

	virtual bool deserializeLengthDelimited(Stream& stream) {
		serialized = true;
		return serialized;
	};

	bool hasSerialized() { return this->serialized; }
	bool wasSuccessful() { return this->successful; }

	virtual void reset() { serialized = false; successful = false; }

protected:

	void fail() { this->successful = false; }
	void success() { this->successful = true; }

	bool serialized;
	bool successful;
};

class BaseNetworkable : public Deserializable {

public:

	// DATA
	uint32_t uid;
	uint32_t group;
	uint32_t prefabID;

	BaseNetworkable() {};
	~BaseNetworkable() {};

	virtual bool deserialize(Stream& stream);
	virtual bool deserializeLengthDelimited(Stream& stream);

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

	virtual bool deserializeLengthDelimited(Stream& stream);

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

	virtual bool deserialize(Stream& stream);

	// first byte is varint32 representing length
	virtual bool deserializeLengthDelimited(Stream& stream);

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

	virtual bool deserialize(Stream& stream);

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

	virtual bool deserializeLengthDelimited(Stream& stream);

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

	virtual bool deserializeLengthDelimited(Stream& stream);

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

	virtual bool deserialize(Stream& stream);

	virtual void reset() {

		Deserializable::reset();
	}
};

};

