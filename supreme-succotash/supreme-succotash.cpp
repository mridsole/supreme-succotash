// supreme-succotash.cpp : Defines the entry point for the console application.

#include "stdafx.h"

#include <stdio.h>
#include <vector>
#include <functional>

#include "SFML/Graphics.hpp"

// pcap utils
#include "pcaputils.h"

// imgui testing
#include "imgui/imgui.h"
#include "imgui/imgui-events-SFML.h"
#include "imgui/imgui-rendering-SFML.h"

#include "EventDispatcher.h"
#include "FrameUpdateDispatcher.h"

#include "GUISelectAdapterWindow.h"

// forward declarations:

// when the adapter device is chosen
void onAdapterChosen(const GUISelectAdapterWindow::Event& _event);

// poll window events and store in the given vector
int pollWindowEvents(sf::Window& window, std::vector<sf::Event>& events);

// storage for selecting adapters
struct {

	std::vector<AdapterDevice> adapters;
	int selectedAdapter;

} selectAdapterState;

int main()
{
	// initialize some state
	selectAdapterState.adapters = getAllAdapterDevices();
	selectAdapterState.selectedAdapter = 0;

	// we need a frame update dispatcher to do much here
	FrameUpdateDispatcher frameUpdateDispatcher;

	// the event to send to the update dispatcher each frame
	FrameUpdateEvent frameUpdateEvent;

	// make the adapter selection window
	GUISelectAdapterWindow selectAdapterWindow(GUISelectAdapterWindow::State(
		&selectAdapterState.adapters,
		&selectAdapterState.selectedAdapter));

	// add callback for click
	selectAdapterWindow.adapterChosenDispatcher.addCallback(&onAdapterChosen);

	sf::RenderWindow window(sf::VideoMode(800, 600), "beep bop");

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

		// ImGuiIO& io = ImGui::GetIO();

		// update our GUIs
		selectAdapterWindow.draw(window.getSize());

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

	// close the adapter selection window
	_event.source->disable();
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