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

class GUISelectAdapterWindow
{

public:

	enum class EventType
	{ AdapterChosen };

	// sent in callbacks
	struct Event {

		// the GUISelectAdapterWindow instance that fired the event
		GUISelectAdapterWindow* source;

		// the type of event
		EventType type;

		// state
		int selectedAdapter;

		Event(GUISelectAdapterWindow* _source, EventType _type, int _selectedAdapter) :
			source(_source),
			type(_type),
			selectedAdapter(_selectedAdapter)
		{}

	};

	// data transfer between user and program
	struct State {

		std::vector<AdapterDevice>* adapters;
		int* selectedAdapter;

		State(std::vector<AdapterDevice>* _adapters,
			int* _selectedAdapter) :
			adapters(_adapters),
			selectedAdapter(_selectedAdapter)
		{}
	};

	GUISelectAdapterWindow(State _state);
	~GUISelectAdapterWindow();

	// just specifies the gui layout
	void draw(const sf::Vector2u& windowSize);

	State state;

	void disable() { this->isEnabled = false; }
	void enable() { this->isEnabled = true; }

	EventDispatcher<const Event&> adapterChosenDispatcher;

private:	

	// should this be drawn?
	bool isEnabled;
};

