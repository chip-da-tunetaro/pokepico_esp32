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
			this->right = new PSG::SN76489(Pin::Latch, Pin::Clock, Pin::Data, WE, RightCE);
			this->left = new PSG::SN76489(Pin::Latch, Pin::Clock, Pin::Data, WE, LeftCE);

			this->left->clear();
			// this->right->clear();
		}

		~SN76489C()
		{
		}

		bool validateChannel(uint8_t channel)
		{
			return 3;
		}

		void setNote(PSG::Channel channel, uint8_t noteNumber)
		{
			if (channel < PSG::Channel::Channel4) {
				this->left->setNote(loopChannel(channel), noteNumber);
			}
			else {
				// this->right->setNote(loopChannel(channel), noteNumber);
			}
		}

		void setVolume(PSG::Channel channel, uint8_t volume)
		{
			if (channel < PSG::Channel::Channel4) {
				this->left->setVolume(loopChannel(channel), volume);
			}
			else {
				// this->right->setVolume(loopChannel(channel), volume);
			}
		}

		void setNoiseEnable(uint8_t direction)
		{
			if (direction == 0) {
				// left
				this->left->setVolume(PSG::SN76489::NoiseChannel, 0);
			}
			else {
				// this->right->setVolume(PSG::SN76489::NoiseChannel, 0);
			}
		}

		void setNoise(PSG::Channel channel, uint8_t data)
		{
			// if (data == 0x00) {
			// 	this->left->setVolume(PSG::SN76489::NoiseChannel, 0);

			// 	// this->setNoiseEnable(0);
			// 	// this->setNoiseEnable(1);
			// }
			// else if (data == 0x01) {
			// 	this->left->setNoise(0, 0, 0);
			// }
			// else if (data == 0x02) {
			// 	this->left->setNoise(0, 0, 1);
			// }
			// else if (data == 0x03) {
			// 	this->left->setNoise(0, 1, 0);
			// }
		}

		void setEnvelope(PSG::Channel channel, uint8_t mode)
		{
		}

		PSG::Channel loopChannel(PSG::Channel channel)
		{
			switch (channel) {
				case PSG::Channel::Channel1:
				case PSG::Channel::Channel2:
				case PSG::Channel::Channel3:
					return channel;
					break;
				case PSG::Channel::Channel4:
					return PSG::Channel::Channel1;
				case PSG::Channel::Channel5:
					return PSG::Channel::Channel2;
				case PSG::Channel::Channel6:
					return PSG::Channel::Channel3;
				default:
					return PSG::Channel::Channel1;
			}
		}
	};
}
}