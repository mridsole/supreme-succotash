#pragma once

#include <vector>
#include <functional>

// NOTE that EventType can (and will be in most cases) something like
// 'const MouseEvent&' (ie a const reference to a struct)
template <typename EventType>
class EventDispatcher
{

private:
	
	// used extensively here
	typedef std::function<void(EventType)> Callback;

	struct CallbackPair { 

		Callback callback;
		unsigned int id;

		// defined so that we can use std::vector::emplace_back
		// (to avoid copying the callback twice when adding it)
		// otherwise, this is POD
		CallbackPair(const Callback& _callback, unsigned int _id):
			callback(_callback), id(_id) {}
	};

public:

	// don't need to do anything here really
	EventDispatcher();
	~EventDispatcher();

	// add a callback to storage (will be copied into std::vector in push_back)
	// returns it's ID
	unsigned int addCallback(const Callback& callback);

	// remove callback from storage
	// returns true if found and removed, false otherwise
	bool removeCallbackByID(unsigned int id);

	// remove all callbacks
	void removeAllCallbacks();

	// returns pointer to the callback, if it exists - otherwise, nullptr
	const Callback* getCallbackByID(unsigned int id) const;

	// fire all callbacks, passing the event through constantly to each one
	void fire(EventType _event);

private:

	std::vector<CallbackPair> callbacks;

	// incremented on every new entry creation
	unsigned int nextID;
};

// implementation below

// usage: Callback<EventType>
// (used in return type stuff)
template <typename EventType>
using Callback = std::function<void(EventType)>;

template <typename EventType>
EventDispatcher<EventType>::EventDispatcher() :
	callbacks(),
	nextID(0u)
{ }

template <typename EventType>
EventDispatcher<EventType>::~EventDispatcher()
{

	// remove all the callbacks first
	this->removeAllCallbacks();
}

template <typename EventType>
unsigned int EventDispatcher<EventType>::addCallback(const Callback& callback) {

	// first update the ID
	this->nextID++;

	// construct new thingy directly in the vector
	this->callbacks.emplace_back(callback, this->nextID);

	// return it's ID
	return this->nextID;
}

template <typename EventType>
bool EventDispatcher<EventType>::removeCallbackByID(unsigned int id) {

	for (unsigned i = 0; i < this->callbacks.size(); i++) {

		if (this->callbacks[i].id == id) {

			// if we find it erase it
			this->callbacks.erase(this->callbacks.begin() + i);
			return true;
		}
	}

	// if we don't find it
	return false;
}

template <typename EventType>
void EventDispatcher<EventType>::removeAllCallbacks() {

	this->callbacks.clear();
}

template <typename EventType>
const Callback<EventType>* EventDispatcher<EventType>::getCallbackByID(unsigned int id) const {

	Callback* callbackPtr = nullptr;

	for (unsigned i = 0; i < this->callbacks.size(); i++) {

		if (this->callbacks[i].id == id) {
			callbackPtr = this->callbacks[i].callback;
		}
	}

	return callbackPtr;
}

template <typename EventType>
void EventDispatcher<EventType>::fire(EventType _event) {

	for (unsigned i = 0; i < this->callbacks.size(); i++) {

		this->callbacks[i].callback(_event);
	}
}
