#pragma once

#include <PSG/SAA1099.h>
#include <pokepico/Cartridge.h>
#include <string>

namespace pokepico
{
namespace Cartridge
{
	using namespace m2d::ESP32::PSG;
	class SAA1099C : public Interface
	{
	private:
		const static gpio_num_t WE = Pin::Conrol1;
		const static gpio_num_t CS = Pin::Conrol2;
		const static gpio_num_t AZ = Pin::Conrol3;

		PSG::SAA1099 *saa;

	public:
		SAA1099C()
		{
			this->saa = new PSG::SAA1099(Pin::Latch, Pin::Clock, Pin::Data, WE, CS, AZ);
			this->saa->reset();
			this->saa->soundEnable();
			this->saa->setNoiseEnable(0);
			for (int i = 0; i < 6; i++) {
				this->saa->setVolume((PSG::Channel)i, SAA1099::OutputSide::Both, 0);
			}
		}

		~SAA1099C()
		{
		}

		bool validateChannel(uint8_t channel)
		{
			return this->saa->validateChannel(channel);
		}

		void setNote(PSG::Channel channel, uint8_t noteNumber)
		{
			this->saa->setNote(channel, noteNumber);
		}

		void setVolume(PSG::Channel channel, uint8_t volume)
		{
			this->saa->setVolume(channel, PSG::SAA1099::OutputSide::Both, volume);
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