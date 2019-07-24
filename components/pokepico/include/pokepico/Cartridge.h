#pragma once

#include <PSG/PSG.h>
#include <driver/gpio.h>

namespace pokepico
{
namespace Cartridge
{
	using namespace m2d::ESP32;
	namespace Pin
	{
		const static gpio_num_t Latch = GPIO_NUM_19;
		const static gpio_num_t Clock = GPIO_NUM_18;
		const static gpio_num_t Data = GPIO_NUM_23;

		const static gpio_num_t Conrol1 = GPIO_NUM_16;
		const static gpio_num_t Conrol2 = GPIO_NUM_17;
		const static gpio_num_t Conrol3 = GPIO_NUM_32;
		const static gpio_num_t Conrol4 = GPIO_NUM_33;
	}

	enum Series : int
	{
		SAA1099 = 0,
		SN76489,
		AY38910,
		Undefined = -1
	};

	class Interface
	{
	public:
		virtual bool validateChannel(uint8_t channel) = 0;
		virtual void setNote(PSG::Channel channel, uint8_t noteNumber) = 0;
		virtual void setVolume(PSG::Channel channel, uint8_t volume) = 0;
	};
};
}