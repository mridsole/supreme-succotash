#pragma once

#include "stdint.h"
#include <thread>

#include "pcaputils.h"

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

	virtual void startHandling(AdapterDevice&);
	virtual void stopHandling();

	virtual void handlePacket(uint8_t* param, const pcap_pkthdr* header, 
		const uint8_t* pkt_data) = 0;

	std::thread& getPcapLoopThread() { return this->pcapLoopThread; }

	void useThreading(bool);

	bool getIsHandling() const { return this->isHandling; }
	bool getIsUsingThreading() const { return this->isUsingThreading; }

private:

	// start the handling loop

	// each handler runs in a pcap_loop in it's own thread
	std::thread pcapLoopThread;

	// is this currently handling?
	bool isHandling;

	// are we using running this handler in a separate thread?
	bool isUsingThreading;
};

