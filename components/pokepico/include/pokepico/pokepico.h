#pragma once

#include <MIDIBLE/MIDIBLE.h>
#include <StreamLogger/Logger.h>
#include <pokepico/Cartridge.h>
#include <string>

#include <pokepico/Cartridge/AY38910.h>
#include <pokepico/Cartridge/SAA1099.h>
#include <pokepico/Cartridge/SN76489.h>

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

	Logger::Group *logger;
	static const gpio_num_t Check1 = GPIO_NUM_34;
	static const gpio_num_t Check2 = GPIO_NUM_35;
	static const gpio_num_t Check3 = GPIO_NUM_36;
	static const gpio_num_t Check4 = GPIO_NUM_39;

	void congiureGPIO()
	{
		this->logger->info << "Congiure GPIO" << Logger::endl;
		gpio_config_t io_conf;
		io_conf.intr_type = (gpio_int_type_t)GPIO_PIN_INTR_DISABLE;
		io_conf.mode = GPIO_MODE_INPUT;
		io_conf.pin_bit_mask = (1ULL << Check1) | (1ULL << Check2) | (1ULL << Check3) | (1ULL << Check4);
		io_conf.pull_down_en = (gpio_pulldown_t)0;
		io_conf.pull_up_en = (gpio_pullup_t)0;
		gpio_config(&io_conf);
	}

	PSG::Channel convertChannel(MIDIBLE::MIDI::Channel channel)
	{
		return static_cast<PSG::Channel>(channel.rawValue() - 1);
	}

	long map(long x, long in_min, long in_max, long out_min, long out_max)
	{
		return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	}

public:
	Device(std::string name, uint16_t udid)
	{
		this->logger = new Logger::Group("pokepico");
		this->congiureGPIO();
		this->registerCartridge();

		this->interface = new MIDIBLE::BLEInterface(name, udid);
		this->interface->note_on_handler = [&](MIDIBLE::MIDI::Channel channel, uint8_t note, uint8_t velocity) {
			auto c = convertChannel(channel.rawValue());
			auto volume = (uint8_t)map(velocity, 0, 128, 0, 15);
			this->logger->info << "on:  "
			                   << "\tMIDIBLE : " << channel.rawValue()
			                   << "\tch : " << c
			                   << "\tnote : " << note
			                   << "\tvelocity: " << velocity
			                   << "\tvolume: " << volume << Logger::endl;
			this->playNote(c, note, volume);
			vTaskDelay(10 / portTICK_PERIOD_MS);
		};

		this->interface->note_off_handler = [&](MIDIBLE::MIDI::Channel channel, uint8_t note, uint8_t velocity) {
			auto c = convertChannel(channel.rawValue());
			this->logger->info << "off: "
			                   << "\tMIDIBLE : " << channel.rawValue()
			                   << "\tch : " << c
			                   << "\tnote : " << note
			                   << "\tvelocity: " << velocity << Logger::endl;
			this->playNote(c, note, 0);
			vTaskDelay(10 / portTICK_PERIOD_MS);
		};

		this->logger->info << "Hello pokepico ;)" << Logger::endl;
	}

	void begin()
	{
		this->interface->begin();
	}

	void playNote(PSG::Channel channel, uint8_t note, uint8_t velocity)
	{
		if (this->cartridge->validateChannel(channel) == false) {
			this->logger->info << "Invalid channel : " << channel << Logger::endl;
			return;
		}
		if (velocity == 0) {
			// off
			this->cartridge->setVolume(channel, 0);
		}
		else {
			this->cartridge->setNote(channel, note);
			this->cartridge->setVolume(channel, velocity);
		}
	}

	Cartridge::Series detectCartridge()
	{
		this->logger->info << "Detect cartridge" << Logger::endl;
		int c1 = gpio_get_level(Check1);
		int c2 = gpio_get_level(Check2);
		this->logger->info << "Check1: " << c1 << ", Check2: " << c2 << Logger::endl;
		if (c2 > 0 && c1 == 0) {
			// SN
			return Cartridge::Series::SN76489;
		}
		else if (c1 > 0 && c2 == 0) {
			// AY
			return Cartridge::Series::AY38910;
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
				this->logger->info << "Register SAA1099" << Logger::endl;
				this->cartridge = new Cartridge::SAA1099C();
				break;
			case Cartridge::Series::SN76489:
				this->logger->info << "Register SN76489" << Logger::endl;
				this->cartridge = new Cartridge::SN76489C();
				break;
			case Cartridge::Series::AY38910:
				this->logger->info << "Register AY38910" << Logger::endl;
				this->cartridge = new Cartridge::AY38910C();
				break;
			case Cartridge::Series::Undefined:
				this->logger->info << "Undefined cartridge :(" << Logger::endl;
				abort();
				break;
		}
	}
};
}