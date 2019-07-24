#pragma once

#include <MIDIBLE/MIDIBLE.h>
#include <StreamLogger/Logger.h>
#include <pokepico/Cartridge.h>
#include <string>

#include <pokepico/Cartridge/SAA1099.h>

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
	Cartridge::Interface *cartridge;
	std::vector<uint8_t> playing_notes;

	static const gpio_num_t Check1 = GPIO_NUM_34;
	static const gpio_num_t Check2 = GPIO_NUM_35;
	static const gpio_num_t Check3 = GPIO_NUM_36;
	static const gpio_num_t Check4 = GPIO_NUM_39;

	void congiureGPIO()
	{
		gpio_config_t io_conf;
		io_conf.intr_type = (gpio_int_type_t)GPIO_PIN_INTR_DISABLE;
		io_conf.mode = GPIO_MODE_INPUT;
		io_conf.pin_bit_mask = ((1ULL << Check1) | (1ULL << Check2) || (1ULL << Check3) | (1ULL << Check4));
		io_conf.pull_down_en = (gpio_pulldown_t)0;
		io_conf.pull_up_en = (gpio_pullup_t)0;
		gpio_config(&io_conf);
	}

public:
	Device(std::string name, uint16_t udid)
	{
		this->congiureGPIO();
		this->registerCartridge();

		this->interface = new MIDIBLE::BLEInterface(name, udid);

		static Logger::Group logger("pokepico");
		this->interface->note_on_handler = [&](MIDIBLE::MIDI::Channel channel, uint8_t note, uint8_t velocity) {
			PSG::Channel c = static_cast<PSG::Channel>(channel.rawValue());
			if (this->cartridge->validateChannel(c) == false) {
				return;
			}
			this->cartridge->setNote(c, note);
			if (velocity == 0) {
				// off
				this->cartridge->setVolume(c, 0);
			}
			else {
				this->cartridge->setVolume(c, velocity);
			}

			logger.info << "on >> \t"
			            << "ch : " << c << "\tnote : " << note << "\tvelocity: " << velocity << Logger::endl;
		};

		this->interface->note_off_handler = [&](MIDIBLE::MIDI::Channel channel, uint8_t note, uint8_t velocity) {
			PSG::Channel c = static_cast<PSG::Channel>(channel.rawValue());
			if (this->cartridge->validateChannel(c) == false) {
				return;
			}
			this->cartridge->setVolume(c, 0);
			logger.info << "off >> \t"
			            << "ch : " << c << "\tnote : " << note << "\tvelocity: " << velocity << Logger::endl;
		};
	}

	void begin()
	{
		this->interface->begin();
	}

	Cartridge::Series detectCartridge()
	{
		int c1 = gpio_get_level(Check1);
		int c2 = gpio_get_level(Check2);
		if (c1 > 0) {
			// AY
			return Cartridge::Series::AY38910;
		}
		else if (c2 > 0) {
			// SN
			return Cartridge::Series::SN76489;
		}
		else {
			// SAA
			return Cartridge::Series::SAA1099;
		}

		return Cartridge::Series::Undefined;
	}

	void registerCartridge()
	{
		switch (this->detectCartridge()) {
			case Cartridge::Series::SAA1099:
				this->cartridge = new Cartridge::SAA1099C();
			case Cartridge::Series::SN76489:
				break;
			case Cartridge::Series::AY38910:
				break;
			case Cartridge::Series::Undefined:
				abort();
				break;
		}
	}
};
}