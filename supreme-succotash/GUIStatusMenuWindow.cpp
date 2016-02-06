#include "stdafx.h"
#include "GUIStatusMenuWindow.h"


GUIStatusMenuWindow::GUIStatusMenuWindow(State _state):
	state(_state),
	isEnabled(true)
{}


GUIStatusMenuWindow::~GUIStatusMenuWindow()
{}


void GUIStatusMenuWindow::draw(const sf::Vector2u& windowSize) {

	if (!this->isEnabled) return;

	int winFlags = ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar;

	// temp bool for state etc

	ImGui::Begin("Status", &this->isEnabled, ImVec2(0, 0), 0.3f, winFlags);

	ImGui::Text("Status");

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	ImGui::Text("You");

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();
	ImGui::Text("Position: ");

	// buffer for client position text
	char posText[200];

	sprintf(posText, "x: %f \ny: %f \nz: %f", 
		state.clientPosition->x, state.clientPosition->y, state.clientPosition->z);

	ImGui::Indent();
	ImGui::Text(posText);
	ImGui::Unindent();

	ImGui::Text("Rotation: ");

	// buffer for client rotation text
	char rotText[200];

	sprintf(rotText, "p: %f \ny: %f \nr: %f",
		state.clientRotation->x, state.clientRotation->y, state.clientRotation->z);

	ImGui::Indent();
	ImGui::Text(rotText);
	ImGui::Unindent();

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Text("Entities");
	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	static int entitySelected = 0;

	// we need to turn the uint16_t identifiers into a char**
	static char buf[10000];
	char const * idStrings[1000];
	int i = 0; int j = 0;

	// (also while we're at it get the IDs themselves
	uint32_t ids[1000];

	for (auto const &pair1 : *(state.entities)) {

		ids[i] = pair1.second.id;

		// if it has a name, show it - otherwise just use ID
		if (pair1.second.hasName) {

			idStrings[i] = pair1.second.name.c_str();
		
		} else {
			idStrings[i] = buf + j;
			j += sprintf(buf + j, "0x%.8x", pair1.second.id) + 1;
		}

		i++;
	}

	ImGui::Spacing();

	// entities list
	ImGui::Indent();
	ImGui::ListBox("##EntitiesListBox", &entitySelected, (const char**)idStrings,
		state.entities->size());

	ImGui::Unindent();

	if (state.entities->size() > 0) {

		// properties of selected entity
		const auto& entity = (*state.entities)[ids[entitySelected]];

		// (just position, for now)
		char entPosText[200];

		sprintf(entPosText, "x: %f \ny: %f \nz: %f",
			entity.position.x, entity.position.y, entity.position.z);

		ImGui::Indent();
		ImGui::Text(entPosText);
		ImGui::Unindent();
	}

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Text("Scale");
	ImGui::Spacing();
	ImGui::SliderFloat("##Scale", state.scale, 0.2, 2);

	ImGui::SetWindowSize(ImVec2(180, windowSize.y - 40));
	ImGui::SetWindowPos(ImVec2(20, 20));

	ImGui::End();
}