#pragma once

#include <iostream>
#include <vector>
#include <string>

#include "pcap.h"

struct AdapterDevice {

	std::string name;
	std::string description;

	pcap_t* handle;

	AdapterDevice(const char* _name, const char* _description) :
		name(_name), description(_description), handle(nullptr)
	{}
};

std::vector<AdapterDevice> getAllAdapterDevices();
int openAdapterDevice(AdapterDevice& device);