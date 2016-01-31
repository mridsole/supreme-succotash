// supreme-succotash.cpp : Defines the entry point for the console application.

#include "stdafx.h"

#include <stdio.h>
#include <vector>
#include <functional>
#include <map>

#include "SFML/Graphics.hpp"

// pcap utils
#include "pcaphandling.h"

// imgui testing
#include "imgui/imgui.h"
#include "imgui/imgui-events-SFML.h"
#include "imgui/imgui-rendering-SFML.h"

#include "EventDispatcher.h"
#include "FrameUpdateDispatcher.h"

#include "GUISelectAdapterWindow.h"
#include "GUIStatusMenuWindow.h"

// testing packet handlers
#include "TestPacketHandler.h"
#include "TickPacketHandler.h"
#include "EntityPositionPacketHandler.h"

// forward declarations:

// when the adapter device is chosen
void onAdapterChosen(const GUISelectAdapterWindow::Event& _event);

// poll window events and store in the given vector
int pollWindowEvents(sf::Window& window, std::vector<sf::Event>& events);

// state storage for selecting adapters
struct {

	std::vector<AdapterDevice> adapters;
	int selectedAdapter;

} selectAdapterState;

// state storage for the status menu
struct {

	sf::Vector3f clientPosition;
	sf::Vector3f clientRotation;
	std::map<uint16_t, sf::Vector3f> entityPositions;
} statusMenuState;

// the status menu window
GUIStatusMenuWindow* statusMenuWindow;

// handles client tick packets
TickPacketHandler* tickPacketHandler = new TickPacketHandler();
EntityPositionPacketHandler* entityPositionPacketHandler = new EntityPositionPacketHandler();

std::vector<PacketHandler*> packetHandlers;

int main()
{
	// which packet handlers we need
	packetHandlers.push_back(tickPacketHandler);
	packetHandlers.push_back(entityPositionPacketHandler);

	// we need a frame update dispatcher to do much here
	FrameUpdateDispatcher frameUpdateDispatcher;

	// the event to send to the update dispatcher each frame
	FrameUpdateEvent frameUpdateEvent;

	// initialize some state
	selectAdapterState.adapters = getAllAdapterDevices();
	selectAdapterState.selectedAdapter = 0;

	// make the adapter selection window
	GUISelectAdapterWindow selectAdapterWindow(GUISelectAdapterWindow::State(
		&selectAdapterState.adapters,
		&selectAdapterState.selectedAdapter));

	// add callback for selecting adapter
	selectAdapterWindow.adapterChosenDispatcher.addCallback(&onAdapterChosen);

	// initialize state for status menu

	// create the status menu window (after initializing it's state)
	statusMenuWindow = new GUIStatusMenuWindow(GUIStatusMenuWindow::State(
		&statusMenuState.clientPosition,
		&statusMenuState.clientRotation,
		&statusMenuState.entityPositions
		));

	// keep this disabled until we've chosen an adapter
	statusMenuWindow->disable();

	sf::RenderWindow window(sf::VideoMode(800, 600), 
		"Zuckles Removal Tool 7.1 Home Premium Edition", sf::Style::Close);

	window.setFramerateLimit(60);

	// SET IMGUI STUFF
	ImGui::SFML::SetWindow(window);
	ImGui::SFML::SetRenderTarget(window);
	ImGui::SFML::InitImGuiRendering();
	ImGui::SFML::InitImGuiEvents();

	while (window.isOpen())
	{
		pollWindowEvents(window, frameUpdateEvent.sfEvents);

		for (unsigned i = 0; i < frameUpdateEvent.sfEvents.size(); i++) {

			// send every event to IMGUI
			ImGui::SFML::ProcessEvent(frameUpdateEvent.sfEvents[i]);

			// close the window if appropriate
			if (frameUpdateEvent.sfEvents[i].type == sf::Event::Closed)
				window.close();
		}

		frameUpdateDispatcher.fire(frameUpdateEvent);

		// IMGUI
		ImGui::SFML::UpdateImGui();
		ImGui::SFML::UpdateImGuiRendering();

		//ImGuiIO& io = ImGui::GetIO();

		// update position state for the menu window gui
		statusMenuState.clientPosition = tickPacketHandler->getLastPlayerPosition();
		statusMenuState.clientRotation = tickPacketHandler->getLastPlayerRotation();
		statusMenuState.entityPositions = entityPositionPacketHandler->getEntityPositions();

		// update our GUIs
		selectAdapterWindow.draw(window.getSize());
		statusMenuWindow->draw(window.getSize());

		static int selectedEntity = 0;

		//ImGui::ListBox("Entities", &selectedEntity, )

		window.clear(sf::Color(20, 30, 40, 255));
		ImGui::Render();
		window.display();
	}

	return 0;
}

void onAdapterChosen(const GUISelectAdapterWindow::Event& _event) {

	AdapterDevice& adapter = selectAdapterState.adapters[_event.selectedAdapter];

	printf("Selected adapter: %s\n", 
		selectAdapterState.adapters[_event.selectedAdapter].description.c_str());

	// open the adapter
	openAdapterDevice(adapter);

	compileSetFilter(adapter, 
		std::string("(src 192.168.0.12 || dst 192.168.0.12) && proto 17"));

	beginHandling(adapter, packetHandlers);

	// close the adapter selection window
	_event.source->disable();

	// open the status menu window
	statusMenuWindow->enable();
}


int pollWindowEvents(sf::Window& window, std::vector<sf::Event>& events) {

	unsigned int nEvent = 0;
	events.resize(1);

	while (window.pollEvent(events[nEvent])) {

		// resize if appropriate
		if (nEvent == events.size() - 1)
			events.resize(events.size() + 1);

		nEvent++;
	}

	if (nEvent == 0)
		events.resize(0);
	else
		events.resize(events.size() - 1);

	return events.size();
}