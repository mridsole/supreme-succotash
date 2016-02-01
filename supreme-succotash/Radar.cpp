#include "stdafx.h"
#include "Radar.h"


Radar::Radar(Radar::State _state, Radar::Config _config):
	state(_state),
	config(_config), 
	enabled(true)
{}


Radar::~Radar()
{}

void Radar::draw(sf::RenderTarget& target, sf::RenderStates states) const {

	if (!enabled) return;

	sf::Vector2f screenSize = sf::Vector2f((float)target.getSize().x, (float)target.getSize().y);

	// render a circle for all entities
	for (const auto& pair : *state.entities) {

		sf::CircleShape entCircle = sf::CircleShape(config.entityRadius);
		entCircle.setFillColor(config.entityColor);
		
		sf::Vector2f fromClient = sf::Vector2f(
			pair.second.position.x - state.client->position.x,
			pair.second.position.z - state.client->position.z
			);

		entCircle.setPosition(sf::Vector2f(
			screenSize.x / 2 + config.scale * fromClient.x, 
			screenSize.y / 2 + config.scale * fromClient.y 
			));

		target.draw(entCircle);
	}

	// render a little circle for the player
	sf::CircleShape clientCircle = sf::CircleShape(config.clientRadius);
	clientCircle.setFillColor(config.clientColor);

	clientCircle.setPosition(sf::Vector2f(screenSize.x / 2, screenSize.y / 2));

	target.draw(clientCircle);
}