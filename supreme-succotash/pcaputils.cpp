#include "pcaputils.h"

#include "PacketHandler.h"

std::vector<AdapterDevice> getAllAdapterDevices() {

	std::vector<AdapterDevice> adapterDevices;

	pcap_if_t*	allDevs;
	char		errorBuffer[PCAP_ERRBUF_SIZE];

	// Find all available devices.
	if ( pcap_findalldevs_ex(PCAP_SRC_IF_STRING, nullptr, &allDevs, errorBuffer) )
		std::cout << "Error in pcap_finalldevs_ex: " << errorBuffer << std::endl;

	int i = 0;
	for (auto* d = allDevs; d != nullptr; d = d->next, ++i) {

		if (d->description != nullptr) {

			adapterDevices.emplace_back(d->name, d->description, 
				d->addresses, d->flags);
		} else {

			adapterDevices.emplace_back(d->name, "No description available.", 
				d->addresses, d->flags);
		}
	}
	
	// maybe don't do this? how do we still refer to ->addresses ???
	// pcap_freealldevs(allDevs);

	return adapterDevices;
}


int openAdapterDevice(AdapterDevice& device) {

	char errorBuffer[PCAP_ERRBUF_SIZE];

	auto adHandle = pcap_open(device.name.c_str(), 65536, 
		PCAP_OPENFLAG_PROMISCUOUS, 1000, nullptr, errorBuffer);

	if (adHandle == nullptr) {

		std::cout << "Unable to open the adapter. " << device.name.c_str()
			<< "is not supported by WinPcap" << std::endl;
		return -1;
	}

	std::cout << "Opened adapter: " << device.name.c_str() << std::endl;

	device.handle = adHandle;

	return 0;
}

// compiles and sets a packet filter - return the compiled filter
bpf_program compileSetFilter(const AdapterDevice& device, std::string filterString) {
	
	// get bok to explain this
	bpf_u_int32 mask = 0;

	if (device.addresses != nullptr) {

		mask = ((sockaddr_in*)(device.addresses->netmask))->sin_addr.S_un.S_addr;
	} else {

		mask = 0xFFFFFFFF;
	}

	// compile the filter then set it
	bpf_program compiledFilter;
	if (pcap_compile(device.handle, &compiledFilter, filterString.c_str(), 1, mask) < 0) {
		
		std::cout << "Unable to compile the packet filter. Check the syntax"
			<< std::endl;
	}

	return compiledFilter;
}

// global state pls go
// belongs to beginHandling and _pcapLoopHandler only!
std::vector<PacketHandler*> handlers;

// sickening
void _pcapLoopHandler(uint8_t* param, const pcap_pkthdr* header,
	const uint8_t* pkt_data) {

	// call all of our handlers
	for (unsigned i = 0; i < handlers.size(); i++) {
		handlers[i]->handlePacket(param, header, pkt_data);
	}
}

// call me in a thread (after compiling and setting filter of course)
std::thread* beginHandling(AdapterDevice& device, std::vector<PacketHandler*> _handlers) {

	handlers = _handlers;

	return new std::thread(pcap_loop, device.handle, 0, _pcapLoopHandler, nullptr);
}