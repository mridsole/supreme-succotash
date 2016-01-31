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
		ImGuiWindowFlags_NoCollapse;

	// temp bool for state etc

	ImGui::Begin("Status", &this->isEnabled, winFlags);

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

	ImGui::SetWindowSize(ImVec2(180, windowSize.y - 40));
	ImGui::SetWindowPos(ImVec2(20, 20));

	ImGui::End();
}