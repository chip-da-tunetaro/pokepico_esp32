#pragma once

#include <MIDIBLE/MIDIBLE.h>
#include <StreamLogger/Logger.h>
#include <string>

namespace pokepico
{
using namespace m2d;
using namespace m2d::ESP32;

class Device
{
private:
	std::string name;
	uint8_t device_udid;
	MIDIBLE::BLEInterface *interface;

public:
	Device(std::string name, uint16_t udid)
	{
		this->interface = new MIDIBLE::BLEInterface(name, udid);

		static Logger::Group logger("pokepico");
		this->interface->note_on_handler = [&](MIDIBLE::MIDI::Channel channel, uint8_t note, uint8_t velocity) {
			logger.info << "on >> \tnote : " << note << ", velocity: " << velocity << Logger::endl;
		};

		this->interface->note_off_handler = [&](MIDIBLE::MIDI::Channel channel, uint8_t note, uint8_t velocity) {
			logger.info << "off >> \tnote : " << note << ", velocity: " << velocity << Logger::endl;
		};
	}

	void begin()
	{
		this->interface->begin();
	}
};
}