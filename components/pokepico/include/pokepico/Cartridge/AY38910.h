#pragma once

#include <PSG/AY38910.h>
#include <pokepico/Cartridge.h>
#include <string>

namespace pokepico
{
namespace Cartridge
{
	using namespace m2d::ESP32::PSG;
	class AY38910C : public Interface
	{
	private:
		const static gpio_num_t BC1 = Pin::Conrol1;
		const static gpio_num_t BCDIR = Pin::Conrol2;
		const static gpio_num_t RST = Pin::Conrol3;

		PSG::AY38910 *ay;

	public:
		AY38910C()
		{
			this->ay = new PSG::AY38910(Pin::Latch, Pin::Clock, Pin::Data, BC1, BCDIR, RST);
		}

		~AY38910C()
		{
		}

		bool validateChannel(uint8_t channel)
		{
			return this->ay->validateChannel(channel);
		}

		void setNote(PSG::Channel channel, uint8_t noteNumber)
		{
			this->ay->setNote(channel, noteNumber);
		}

		void setVolume(PSG::Channel channel, uint8_t volume)
		{
			this->ay->setVolume(channel, volume);
		}

		void setNoiseEnable(uint8_t channelbit)
		{
		}

		void setNoise(PSG::Channel channel, uint8_t data)
		{
			this->ay->setNoise(data);
		}

		void setEnvelope(PSG::Channel channel, uint8_t mode)
		{
		}
	};
}
}