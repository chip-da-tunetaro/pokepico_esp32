#pragma once

#include <MIDIBLE/MIDIBLE.h>
#include <StreamLogger/Logger.h>
#include <pokepico/Cartridge.h>
#include <string>

#include <pokepico/Cartridge/AY38910.h>
#include <pokepico/Cartridge/SAA1099.h>
#include <pokepico/Cartridge/SN76489.h>

#include <bitset>

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

	static const gpio_num_t LED1 = GPIO_NUM_12;
	static const gpio_num_t LED2 = GPIO_NUM_14;
	static const gpio_num_t LED3 = GPIO_NUM_15;

	static const gpio_num_t Key = GPIO_NUM_27;

	bool midi_received = false;

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

	void configureLED()
	{
		this->logger->info << "Congiure LED" << Logger::endl;
		gpio_config_t io_conf;
		io_conf.intr_type = (gpio_int_type_t)GPIO_PIN_INTR_DISABLE;
		io_conf.mode = GPIO_MODE_OUTPUT;
		io_conf.pin_bit_mask = (1ULL << GPIO_NUM_12) | (1ULL << GPIO_NUM_14) | (1ULL << GPIO_NUM_15);
		io_conf.pull_down_en = (gpio_pulldown_t)0;
		io_conf.pull_up_en = (gpio_pullup_t)0;
		gpio_config(&io_conf);
	}

	PSG::Channel convertChannel(MIDIBLE::MIDI::Channel channel)
	{
		return static_cast<PSG::Channel>(channel.rawValue() - 1);
	}

public:
	Device(std::string name)
	{
		this->name = name;
		this->logger = new Logger::Group("pokepico");
		this->congiureGPIO();
		this->configureLED();

		this->setLedState(LED1, false);
		this->setLedState(LED2, false);
		this->setLedState(LED3, false);
	}

	long map(long x, long in_min, long in_max, long out_min, long out_max)
	{
		return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
	}

	void begin()
	{
		this->startLedTask();
		this->logger->info << "Start BLE" << Logger::endl;
		this->startBluetooth();
		this->logger->info << "Register cartridge" << Logger::endl;
		this->registerCartridge();
		this->logger->info << "Hello pokepico ;)" << Logger::endl;
		this->setLedState(LED2, true);
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

	void setLedState(gpio_num_t led, bool on)
	{
		if (led < LED1 && led > LED3) {
			return;
		}
		if (on) {
			gpio_set_level(led, 1);
		}
		else {
			gpio_set_level(led, 0);
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

	void startLedTask()
	{
		static m2d::FreeRTOS::Task led_task("LED Task", 10, 1024 * 3, [&] {
			while (1) {
				// Do something
				if (this->midi_received) {
					this->setLedState(LED1, true);
					vTaskDelay(50 / portTICK_PERIOD_MS);
					this->setLedState(LED1, false);
					this->midi_received = false;
				}
				vTaskDelay(50 / portTICK_PERIOD_MS);
			}
		});
		led_task.run();
	}

	void startBluetooth()
	{
		this->interface = new MIDIBLE::BLEInterface(this->name, 0x0c);
		this->interface->onConnected([&]() {
			this->setLedState(LED3, true);
		});
		this->interface->onDisconnected([&]() {
			this->setLedState(LED3, false);
		});
		this->interface->note_on_handler = [&](MIDIBLE::MIDI::Channel channel, uint8_t note, uint8_t velocity) {
			auto c = convertChannel(channel.rawValue());
			this->logger->info << "on:  "
			                   << "\tMIDIBLE : " << channel.rawValue()
			                   << "\tch : " << c
			                   << "\tnote : " << note
			                   << "\tvelocity: " << velocity << Logger::endl;
			this->playNote(c, note, velocity);
			this->midi_received = true;
		};
		this->interface->note_off_handler = [&](MIDIBLE::MIDI::Channel channel, uint8_t note, uint8_t velocity) {
			auto c = convertChannel(channel.rawValue());
			this->logger->info << "off: "
			                   << "\tMIDIBLE : " << channel.rawValue()
			                   << "\tch : " << c
			                   << "\tnote : " << note
			                   << "\tvelocity: " << velocity << Logger::endl;
			this->playNote(c, note, 0);
		};
		vTaskDelay(250 / portTICK_PERIOD_MS);
		this->interface->begin();
	}
};
}