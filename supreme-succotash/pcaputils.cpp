#include "pcaputils.h"


std::vector<AdapterDevice> getAllAdapterDevices() {

	std::vector<AdapterDevice> adapterDevices;

	pcap_if_t*	allDevs;
	char		errorBuffer[PCAP_ERRBUF_SIZE];

	// Find all available devices.
	if ( pcap_findalldevs_ex(PCAP_SRC_IF_STRING, nullptr, &allDevs, errorBuffer) )
		std::cout << "Error in pcap_finalldevs_ex: " << errorBuffer << std::endl;

	int i = 0;
	for (auto* d = allDevs; d != nullptr; d = d->next, ++i) {

		if (d->description != nullptr)
			adapterDevices.emplace_back(d->name, d->description);
		else
			adapterDevices.emplace_back(d->name, "No description available.");
	}

	pcap_freealldevs(allDevs);

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

// ONLY CONTAINS PCAP SETUP
//int yer()
//{
//	bpf_u_int32 mask = 0;
//	if (d->addresses != nullptr)
//	{
//		mask =
//			((sockaddr_in*)(d->addresses->netmask))->sin_addr.S_un.S_addr;
//	} else
//	{
//		mask = 0xFFFFFFFF;
//	}
//
//	// Compile and set the packet filter.
//	bpf_program code;
//	if (pcap_compile(adHandle, &code, FilterString, 1, mask) < 0)
//	{
//		std::cout << "Unable to compile the packet filter. Check the syntax"
//			<< std::endl;
//		return -1;
//	}
//
//	if (pcap_setfilter(adHandle, &code) < 0)
//	{
//		std::cout << "Error setting the filter." << std::endl;
//		return -1;
//	}
//
//	// change handler as desired
//	pcap_loop(adHandle, 0, PacketHandler_CodelockCrackerUnreliable, nullptr);
//
//	return 0;
//}

