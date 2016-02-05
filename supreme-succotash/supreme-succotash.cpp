// supreme-succotash.cpp : Defines the entry point for the console application.

#include "stdafx.h"

#include <stdio.h>
#include <vector>
#include <functional>
#include <map>

#include "SFML/Graphics.hpp"

// pcap utils
#include "pcaphandling.h"

// imgui
#include "imgui/imgui.h"
#include "imgui/imgui-events-SFML.h"
#include "imgui/imgui-rendering-SFML.h"

// event dispatching
#include "EventDispatcher.h"
#include "FrameUpdateDispatcher.h"

// gui markup
#include "GUISelectAdapterWindow.h"
#include "GUIStatusMenuWindow.h"

// testing packet handlers
#include "TestPacketHandler.h"
#include "TickPacketHandler.h"
#include "EntityPositionPacketHandler.h"
#include "EntitiesPacketHandler.h"

// for rendering entity positions
#include "Radar.h"

// for testing, currently
#include "protobufutils.h"
#include "protobufdeserializers.h"

// forward declarations:

// when the adapter device is chosen
void onAdapterChosen(const GUISelectAdapterWindow::Event& _event);

// poll window events and store in the given vector
int pollWindowEvents(sf::Window& window, std::vector<sf::Event>& events);

void testEntitiesPacket(const uint8_t* pkt_data, size_t len);
void testEntityPositionPacket(const uint8_t* pkt_data, size_t len);
void testTickPacket(const uint8_t* pkt_data, size_t len);


// state storage for selecting adapters
struct {

	std::vector<AdapterDevice> adapters;
	int selectedAdapter;

} selectAdapterState;

// state storage for the status menu
struct {

	sf::Vector3f clientPosition;
	sf::Vector3f clientRotation;
	TickPacketHandler::ClientState clientState;
	EntityPositionPacketHandler::EntityMap entities;
	float scale;
} statusMenuState;

// the status menu window
GUIStatusMenuWindow* statusMenuWindow;

// handles client tick packets
TickPacketHandler* tickPacketHandler = new TickPacketHandler();
EntityPositionPacketHandler* entityPositionPacketHandler = new EntityPositionPacketHandler();
EntitiesPacketHandler* entitiesPacketHandler = new EntitiesPacketHandler(&statusMenuState.entities);

// pointers to all our packet handlers
std::vector<PacketHandler*> packetHandlers;

// draws the stuff
Radar* radar;

int main()
{
	// TESTING

	// load in a test packet
	FILE* fileptr = fopen("testdata/newticktest", "rb");
	fseek(fileptr, 0, SEEK_END);
	size_t len = ftell(fileptr);
	rewind(fileptr);

	uint8_t data[2000];
	fread(data, len, 1, fileptr);

	testTickPacket(data, len);

	// END TESTING

	// which packet handlers we need
	packetHandlers.push_back(tickPacketHandler);
	packetHandlers.push_back(entityPositionPacketHandler);
	packetHandlers.push_back(entitiesPacketHandler);

	// we need a frame update dispatcher to do much here
	FrameUpdateDispatcher frameUpdateDispatcher;

	// the event to send to the update dispatcher each frame
	FrameUpdateEvent frameUpdateEvent;

	// initialize some state
	selectAdapterState.adapters = getAllAdapterDevices();
	selectAdapterState.selectedAdapter = 0;

	statusMenuState.scale = 1.f;

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
		&statusMenuState.entities,
		&statusMenuState.scale
		));

	// keep this disabled until we've chosen an adapter
	statusMenuWindow->disable();

	// initialize the radar
	Radar::State radarState = Radar::State(&statusMenuState.entities, &statusMenuState.clientState);
	Radar::Config radarConfig;
	radarConfig.clientColor = sf::Color(60, 200, 60, 255);
	radarConfig.entityColor = sf::Color(200, 60, 60, 255);
	radarConfig.clientRadius = 5.f;
	radarConfig.entityRadius = 5.f;
	radarConfig.scale = 1.f;

	radar = new Radar(radarState, radarConfig);

	// disable the radar for now
	radar->disable();

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

		// update position state for the menu window gui
		statusMenuState.clientPosition = tickPacketHandler->getLastPlayerPosition();
		statusMenuState.clientRotation = tickPacketHandler->getLastPlayerRotation();
		statusMenuState.clientState = tickPacketHandler->getClientState();
		statusMenuState.entities = entityPositionPacketHandler->getEntities();


		// IMGUI
		ImGui::SFML::UpdateImGui();
		ImGui::SFML::UpdateImGuiRendering();

		//ImGuiIO& io = ImGui::GetIO();

		// update our GUIs
		selectAdapterWindow.draw(window.getSize());
		statusMenuWindow->draw(window.getSize());

		static int selectedEntity = 0;

		//ImGui::ListBox("Entities", &selectedEntity, )

		window.clear(sf::Color(20, 30, 40, 255));

		// draw the radar
		radar->draw(window, sf::RenderStates::Default);
		// and set it's scale
		radar->setScale(statusMenuState.scale);

		// render IMGUI
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
		std::string("(src 192.168.1.7 || dst 192.168.1.7) && proto 17"));

	beginHandling(adapter, packetHandlers);

	// close the adapter selection window
	_event.source->disable();

	// open the status menu window
	statusMenuWindow->enable();
	
	// enable the radar
	radar->enable();
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

void testEntitiesPacket(const uint8_t* pkt_data, size_t len) {

	deserialize::Entity entityDeserializer;

	// advance to the start of the entity
	Stream stream = Stream(pkt_data, len);
	advance(stream, 74);

	// NOTE: the 0a is the start of a base networkable - the 0c is the length
	// of the base networkable, the 08 is the first byte written by the base networkable
	constexpr uint8_t searchFor[] = { 0x0a, 0x0c, 0x08 };

	while (stream.bytes - stream.bytesStart < stream.len - 4) {

		// reset the deserializer
		entityDeserializer.reset();

		// advance to the next entity
		uint8_t const * nextEntity = std::search(stream.bytes, stream.bytesEnd, 
			searchFor, searchFor + 3);

		if (nextEntity == stream.bytesEnd) break;

		advance(stream, nextEntity - stream.bytes);

		// deserialize it
		entityDeserializer.deserialize(stream);

		// if we got a BasePlayer from it, print out the stats
		if (entityDeserializer.baseNetworkable.hasSerialized() &&
			entityDeserializer.basePlayer.hasSerialized()) {

			printf("Got a player, id: %.8x - name: %s\n",
				entityDeserializer.baseNetworkable.uid,
				entityDeserializer.basePlayer.name);
		}
	}
}

void testEntityPositionPacket(const uint8_t* pkt_data, size_t len) {

	// stream for our packet
	Stream stream = Stream(pkt_data, len);
	advance(stream, 66);

	uint32_t entityID = readUInt32(stream);

	float x = readFloat(stream);
	float y = readFloat(stream);
	float z = readFloat(stream);

	printf("hi");
}

void testTickPacket(const uint8_t* pkt_data, size_t len) {

	Stream stream = Stream(pkt_data, len);
	advance(stream, 70);

	deserialize::PlayerTick tickDeserializer;

	tickDeserializer.deserialize(stream);

	printf("done!\n");
}