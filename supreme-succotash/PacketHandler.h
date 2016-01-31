#pragma once

#include "stdint.h"
#include <thread>

#include "pcaphandling.h"

// interface for responding to observed packets that match the filter
class PacketHandler
{

public:

	PacketHandler()
		:pcapLoopThread(),
		isHandling(false),
		isUsingThreading(false)
		{};

	virtual ~PacketHandler() {};

	virtual void onStartHandling();
	virtual void onStopHandling();

	virtual void handlePacket(uint8_t* param, const pcap_pkthdr* header, 
		const uint8_t* pkt_data) = 0;

	bool getIsHandling() const { return this->isHandling; }

private:

	// start the handling loop

	// each handler runs in a pcap_loop in it's own thread
	std::thread pcapLoopThread;

	// is this currently handling?
	bool isHandling;

	// are we using running this handler in a separate thread?
	bool isUsingThreading;
};

