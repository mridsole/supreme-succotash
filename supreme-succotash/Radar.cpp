#include "stdafx.h"
#include "Radar.h"

#include <cmath>

#define DEG_2_RAD 0.0174533

Radar::Radar(Radar::State _state, Radar::Config _config):
	state(_state), 
	config(_config), 
	enabled(true),
	font()
{
	font.loadFromFile("proggyclean.ttf");
}


Radar::~Radar()
{}

void Radar::draw(sf::RenderTarget& target, sf::RenderStates states) const {

	if (!enabled) return;

	sf::Vector2f screenSize = sf::Vector2f((float)target.getSize().x, (float)target.getSize().y);

	// render a circle for all entities
	for (const auto& pair : *state.entities) {

		sf::CircleShape entCircle = getEntityCircle(pair.second, screenSize);
		target.draw(entCircle);

		// also draw the ID of the entity next to it
		char entStr[512]; sprintf(entStr, "0x%.4x\n", pair.second.id);
		if (pair.second.hasName)
			strcpy(entStr + 7, pair.second.name.c_str());

		sf::Text idText = sf::Text(sf::String(entStr), font, 14);

		idText.setPosition(sf::Vector2f(entCircle.getPosition().x + 15, entCircle.getPosition().y));
		idText.setColor(sf::Color(255, 255, 255, 255));
		target.draw(idText);
	}

	// render a line pointing in the client's view direction
	target.draw(getClientViewLine(*state.client, screenSize));

	// render a circle for the player
	target.draw(getClientCircle(*state.client, screenSize));
}

sf::CircleShape Radar::getEntityCircle(const EntityPositionPacketHandler::Entity& entity,
	sf::Vector2f& screenSize) const {

	sf::CircleShape entCircle = sf::CircleShape(config.entityRadius);
	entCircle.setFillColor(config.entityColor);

	// their coordinate system is munted, so account for that
	sf::Vector2f fromClient = sf::Vector2f(
		entity.position.x - state.client->position.x,
		entity.position.z - state.client->position.z
		);

	// don't forget to take into account the size of the circle itself
	entCircle.setPosition(sf::Vector2f(
		screenSize.x / 2 + fromClient.x / config.scale - config.entityRadius,
		screenSize.y / 2 - fromClient.y / config.scale - config.entityRadius
		));

	return entCircle;
}

sf::CircleShape Radar::getClientCircle(TickPacketHandler::ClientState& client,
	sf::Vector2f& screenSize) const {

	// render a circle for the player
	sf::CircleShape clientCircle = sf::CircleShape(config.clientRadius);
	clientCircle.setFillColor(config.clientColor);

	// account for circle size
	clientCircle.setPosition(sf::Vector2f(screenSize.x / 2 - config.clientRadius, 
		screenSize.y / 2 - config.clientRadius));

	return clientCircle;
}

sf::VertexArray Radar::getClientViewLine(TickPacketHandler::ClientState& client,
	sf::Vector2f& screenSize) const {

	// draw a line representing the player's view
	sf::VertexArray clientViewLine = sf::VertexArray();
	clientViewLine.append(sf::Vertex(sf::Vector2f(screenSize.x / 2, screenSize.y / 2), config.clientColor));

	// the second point
	sf::Vertex clientViewPoint2 = sf::Vertex(
		sf::Vector2f(screenSize.x / 2 + 500 * std::sin(DEG_2_RAD * state.client->rotation.y),
			screenSize.y / 2 - 500 * std::cos(DEG_2_RAD * state.client->rotation.y)),
		config.clientColor);

	clientViewLine.append(clientViewPoint2);

	clientViewLine.setPrimitiveType(sf::LinesStrip);

	return clientViewLine;
}