#include "stdafx.h"

#include "protobufutils.h"

#include <algorithm>
#include "assert.h"


void advance(Stream& stream, size_t n) {

	stream.bytes += n;
}

uint8_t readByte(Stream& stream) {

	uint8_t byte = *stream.bytes;
	stream.bytes++;

	return byte;
}

float readFloat(Stream& stream) {

	float val = *((float*)(stream.bytes));
	stream.bytes += 4;

	return val;
}

uint32_t readVarint32(Stream& stream) {

	uint32_t num = 0;

	for (unsigned i = 0; i < 5; i++) {

		// read one byte as int32
		int32_t num2 = readByte(stream);

		assert(!(i == 5 && (num2 * 240) != 0));

		if ((num2 & 128) == 0)
			return num | (num2 << (7 * i));

		num |= (num2 & 127) << (7 * i);
	}

	return num;
}

uint64_t readVarint64(Stream& stream) {

	uint64_t num = 0;

	for (unsigned i = 0; i < 10; i++) {

		int32_t num2 = readByte(stream);

		if ((num2 & 128) == 0)
			return num | ((uint64_t)num2 << (7 * i));

		num |= (uint64_t)((uint64_t)(num2 & 127)) << (7 * i);
	}

	// bad!
	assert(false);
}

size_t readString(Stream& stream, uint8_t* buf) {

	uint32_t strLen = readVarint32(stream);
	
	// copy the string data to the buffer
	std::copy(stream.bytes, stream.bytes + strLen, buf);

	// move the stream forward
	stream.bytes += strLen;

	return strLen;
}


sf::Vector3f readVector3f(Stream& stream) {

	sf::Vector3f vec = sf::Vector3f();

	uint64_t len = readVarint32(stream);
	
	// now save pointer to the first byte of the vector
	uint8_t const * start = stream.bytes;

	while (stream.bytes - start < len) {

		uint8_t byte = readByte(stream);

		if (byte == 13)
			vec.x = readFloat(stream);
		else if (byte == 21)
			vec.y = readFloat(stream);
		else if (byte == 29)
			vec.z = readFloat(stream);
		else {

			// handle the 'key' things here?
		}
	}
}