#include "stdafx.h"
#include "GUISelectAdapterWindow.h"


GUISelectAdapterWindow::GUISelectAdapterWindow(State _state):
	state(_state), 
	isEnabled(true),
	adapterChosenDispatcher()
{ }


GUISelectAdapterWindow::~GUISelectAdapterWindow()
{ }


void GUISelectAdapterWindow::draw(const sf::Vector2u& windowSize) {

	if (!this->isEnabled) return;

	int winFlags = ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_AlwaysAutoResize;

	// temp bool for state etc

	ImGui::Begin("Select Network Adapter Device", &this->isEnabled, winFlags);

	ImGui::Text("Select Network Adapter Device");

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	std::vector<AdapterDevice>& adapters = *state.adapters;

	for (unsigned i = 0; i < adapters.size(); i++) {

		ImGui::RadioButton(
			adapters[i].name.c_str(), 
			state.selectedAdapter, 
			i );

		ImGui::Text(adapters[i].description.c_str());

		ImGui::Spacing();
	}

	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Spacing();
	
	ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 55);
	if (ImGui::Button("Select")) {

		Event aEvent(this, EventType::AdapterChosen, *state.selectedAdapter);
		adapterChosenDispatcher.fire(aEvent);
	}

	// center the window
	ImVec2 thisSize = ImGui::GetWindowSize();
	ImGui::SetWindowPos(ImVec2(windowSize.x / 2 - thisSize.x / 2,
		windowSize.y / 2 - thisSize.y / 2));

	ImGui::End();
}