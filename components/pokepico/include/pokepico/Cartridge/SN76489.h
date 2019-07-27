#pragma once

#include <PSG/SN76489.h>
#include <pokepico/Cartridge.h>
#include <string>

namespace pokepico
{
namespace Cartridge
{
	using namespace m2d::ESP32::PSG;
	class SN76489C : public Interface
	{
	private:
		const static gpio_num_t WE = Pin::Conrol1;
		const static gpio_num_t LeftCE = Pin::Conrol2;
		const static gpio_num_t RightCE = Pin::Conrol3;

		PSG::SN76489 *left;
		PSG::SN76489 *right;

	public:
		SN76489C()
		{
			this->left = new PSG::SN76489(Pin::Latch, Pin::Clock, Pin::Data, WE, LeftCE);
			this->right = new PSG::SN76489(Pin::Latch, Pin::Clock, Pin::Data, WE, RightCE);

			this->left->clear();
			this->right->clear();
		}

		~SN76489C()
		{
		}

		bool validateChannel(uint8_t channel)
		{
			return this->left->validateChannel(channel);
		}

		void setNote(PSG::Channel channel, uint8_t noteNumber)
		{
			this->left->setNote(channel, noteNumber);
			this->right->setNote(channel, noteNumber);
		}

		void setVolume(PSG::Channel channel, uint8_t volume)
		{
			this->left->setVolume(channel, volume);
			this->right->setVolume(channel, volume);
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