#include "stdafx.h"

#include <functional>

#include "PacketHandler.h"


void PacketHandler::onStartHandling() {

	if (this->isHandling) return;

	this->isHandling = true;
}

void PacketHandler::onStopHandling() {

	// this is checked in the handler
	this->isHandling = false;
}
