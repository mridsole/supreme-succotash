#pragma once

#include "SFML/Graphics.hpp"

#include "EntityPositionPacketHandler.h"
#include "TickPacketHandler.h"

class Radar : public sf::Drawable
{

public:

	// references to drawing info
	struct State {

		EntityPositionPacketHandler::EntityMap* entities;
		TickPacketHandler::ClientState* client;

		State(EntityPositionPacketHandler::EntityMap* _entities, 
			TickPacketHandler::ClientState* _client) :
			entities(_entities),
			client(_client)
		{}
	};

	// config for drawing entities
	struct Config {

		float clientRadius;
		sf::Color clientColor;

		float entityRadius;
		sf::Color entityColor;

		// ingame units to pixels
		float scale;

		Config(float _clientRadius, sf::Color _clientColor,
			float _entityRadius, sf::Color _entityColor, float _scale) 
			:
			clientRadius(_clientRadius), clientColor(_clientColor), 
			entityRadius(_entityRadius), entityColor(_entityColor), scale(_scale)
		{}

		Config() {}
	};

	Radar(State _state, Config _config);
	~Radar();

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;

	const Config& getConfig() const {
		return config;
	}

	void setConfig(Config& _config) {
		config = _config;
	}

	bool isEnabled() { return enabled; }
	void enable() { enabled = true; }
	void disable() { enabled = false; }


private:

	sf::CircleShape getEntityCircle(const EntityPositionPacketHandler::Entity& entity,
		sf::Vector2f& screenSize) const;

	sf::CircleShape getClientCircle(TickPacketHandler::ClientState& client,
		sf::Vector2f& screenSize) const;

	sf::VertexArray getClientViewLine(TickPacketHandler::ClientState& client,
		sf::Vector2f& screenSize) const;

	State state;
	Config config;

	// should draw?
	bool enabled;
};

