#pragma once

#include <vector>

#include <SFML/Graphics.hpp>

#include "EventDispatcher.h"

#include "imgui/imgui.h"
#include "imgui/imgui-events-SFML.h"
#include "imgui/imgui-rendering-SFML.h"

#include <vector>
#include <string>

#include "pcaphandling.h"

class GUIStatusMenuWindow
{

public:

	enum class EventType
	{
		AdapterChosen
	};

	// sent in callbacks
	struct Event {

		// the GUISelectAdapterWindow instance that fired the event
		GUIStatusMenuWindow* source;

		// the type of event
		EventType type;

		Event(GUIStatusMenuWindow* _source, EventType _type) :
			source(_source),
			type(_type)
		{}

	};

	// data transfer between user and program
	struct State {

		sf::Vector3f* clientPosition;
		sf::Vector3f* clientRotation;

		State(sf::Vector3f* _clientPosition, sf::Vector3f* _clientRotation):
			clientPosition(_clientPosition),
			clientRotation(_clientRotation)
		{}
	};

	GUIStatusMenuWindow(State _state);
	~GUIStatusMenuWindow();

	// just specifies the gui layout
	void draw(const sf::Vector2u& windowSize);

	State state;

	void disable() { this->isEnabled = false; }
	void enable() { this->isEnabled = true; }

private:

	// should this be drawn?
	bool isEnabled;
};