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

	ImGui::Begin("Status", &this->isEnabled, winFlags);

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

	sprintf(rotText, "x: %f \ny: %f \nz: %f",
		state.clientRotation->x, state.clientRotation->y, state.clientRotation->z);

	ImGui::Indent();
	ImGui::Text(rotText);
	ImGui::Unindent();

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Text("Entities");
	ImGui::Separator();
	ImGui::Spacing();

	static int entitySelected = 0;

	// we need to turn the uint16_t identifiers into a char**
	char buf[10000];
	char* buf2buf[1000];
	int i = 0; int j = 0;

	for (auto const &pair1 : *(state.entityPositions)) {

		buf2buf[i] = buf + j;
		j += sprintf(buf + j, "0x%.2x", pair1.first) + 1;

		i++;
	}

	ImGui::ListBox("#EntitiesListBox", &entitySelected, (const char**)buf2buf,
		state.entityPositions->size());

	ImGui::SetWindowSize(ImVec2(180, windowSize.y - 40));
	ImGui::SetWindowPos(ImVec2(20, 20));

	ImGui::End();
}