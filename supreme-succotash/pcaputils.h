#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <thread>

#include "pcap.h"

class PacketHandler;

struct AdapterDevice {

	std::string name;
	std::string description;
	pcap_addr* addresses;
	bpf_u_int32 flags;

	pcap_t* handle;

	AdapterDevice(const char* _name, 
		const char* _description, 
		pcap_addr* _addresses, 
		bpf_u_int32 _flags) 
		:
		name(_name), 
		description(_description), 
		addresses(_addresses), 
		flags(_flags), 
		handle(nullptr)
	{}
};

std::vector<AdapterDevice> getAllAdapterDevices();
int openAdapterDevice(AdapterDevice& device);
bpf_program compileSetFilter(const AdapterDevice& device, std::string filterString);
std::thread* beginHandling(AdapterDevice& device, std::vector<PacketHandler*> handlers);