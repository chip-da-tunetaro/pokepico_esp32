#pragma once

#include <PSG/SAA1099.h>
#include <pokepico/Cartridge.h>
#include <string>

namespace pokepico
{
namespace Cartridge
{
	using namespace m2d;
	using namespace m2d::ESP32;
	class SAA1099C : public Interface
	{
	private:
		const static gpio_num_t WE = Pin::Conrol1;
		const static gpio_num_t CS = Pin::Conrol2;
		const static gpio_num_t AZ = Pin::Conrol3;

		ESP32::SAA1099 *saa;

	public:
		SAA1099C()
		{
			this->saa = new ESP32::SAA1099(Pin::Latch, Pin::Clock, Pin::Data, WE, CS, AZ);
			this->saa->reset();
			this->saa->soundEnable();
			this->saa->setVolume(PSG::Channel::c1, SAA1099::OutputSide::Both, 0);
			this->saa->setVolume(PSG::Channel::c2, SAA1099::OutputSide::Both, 0);
			this->saa->setVolume(PSG::Channel::c3, SAA1099::OutputSide::Both, 0);
			this->saa->setVolume(PSG::Channel::c4, SAA1099::OutputSide::Both, 0);
			this->saa->setVolume(PSG::Channel::c5, SAA1099::OutputSide::Both, 0);
			this->saa->setVolume(PSG::Channel::c6, SAA1099::OutputSide::Both, 0);
		}

		~SAA1099C()
		{
		}

		bool validateChannel(uint8_t channel)
		{
			return this->validateChannel(channel);
		}

		void setNote(PSG::Channel channel, uint8_t noteNumber)
		{
			this->saa->setNote(channel, noteNumber);
		}

		void setVolume(PSG::Channel channel, uint8_t volume)
		{
			this->saa->setVolume(channel, m2d::ESP32::SAA1099::OutputSide::Both, volume);
		}

		void setNoiseEnable(uint8_t channelbit)
		{
		}

		void setNoise(PSG::Channel channel, uint8_t mode)
		{
		}

		void setEnvelope(PSG::Channel channel, uint8_t mode)
		{
		}
	};
}
}