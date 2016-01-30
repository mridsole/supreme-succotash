#include "stdafx.h"

#include <functional>

#include "PacketHandler.h"


void PacketHandler::startHandling(AdapterDevice& device) {

	if (this->isHandling) return;

	this->isHandling = true;

	//// function pointer to our handlePacket callback
	//auto handler = std::mem_fn(PacketHandler::handlePacket);
	//auto bindedHandler = std::bind(handler, this, std::placeholders::_1, 
	//	std::placeholders::_2, std::placeholders::_3);


	//// start the pcap loop in our stored thread
	//pcapLoopThread = std::thread(pcap_loop, device.handle, 0, bindedHandler, nullptr);
}

void PacketHandler::stopHandling() {

	// this is checked in the handler
	this->isHandling = false;
}

void PacketHandler::useThreading(bool use) {

	if (this->isHandling) return;

	this->isUsingThreading = true;
}