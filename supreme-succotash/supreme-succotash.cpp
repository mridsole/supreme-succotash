// supreme-succotash.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <vector>
#include <functional>

#include "SFML/Graphics.hpp"

// imgui testing
#include "imgui/imgui.h"
#include "imgui/imgui-events-SFML.h"
#include "imgui/imgui-rendering-SFML.h"

#include "EventDispatcher.h"
#include "FrameUpdateDispatcher.h"

// some utility functions for dealing with windows
// should really reserve some memory first in the events vector
int pollWindowEvents(sf::Window& window, std::vector<sf::Event>& events);

int main()
{
	// we need a frame update dispatcher to do much here
	FrameUpdateDispatcher frameUpdateDispatcher;

	// for efficiency
	FrameUpdateEvent frameUpdateEvent;
	frameUpdateEvent.sfEvents.resize(1);

	sf::RenderWindow window(sf::VideoMode(1024, 768), "Beep Bop");

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

		ImGuiIO& io = ImGui::GetIO();

		ImGui::ShowTestWindow();

		window.clear(sf::Color(255, 255, 255, 255));
		ImGui::Render();
		window.display();
	}

	return 0;
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