/*
 * led.hpp
 *
 *  Created on: 24 нояб. 2020 г.
 *      Author: Alex
 */

#ifndef INCLUDE_LED_HPP_
#define INCLUDE_LED_HPP_

#include <cstring>
#include "gpio.hpp"
#include "rcc.hpp"

class led_c {

public:
	static void Init() noexcept {
		SetAll(1);
		gpio::ConfigAsOut(gpio::GPIO_PORT_t::PORTA, 4, gpio::GPIO_SPEED_t::LOW,
						gpio::GPIO_OUT_TYPE_t::PP, gpio::GPIO_PULL_TYPE_t::UP);
		gpio::ConfigAsOut(gpio::GPIO_PORT_t::PORTA, 6, gpio::GPIO_SPEED_t::LOW,
						gpio::GPIO_OUT_TYPE_t::PP, gpio::GPIO_PULL_TYPE_t::UP);
		gpio::ConfigAsOut(gpio::GPIO_PORT_t::PORTA, 2, gpio::GPIO_SPEED_t::LOW,
						gpio::GPIO_OUT_TYPE_t::PP, gpio::GPIO_PULL_TYPE_t::UP);
		gpio::ConfigAsOut(gpio::GPIO_PORT_t::PORTA, 5, gpio::GPIO_SPEED_t::LOW,
						gpio::GPIO_OUT_TYPE_t::PP, gpio::GPIO_PULL_TYPE_t::UP);
		gpio::ConfigAsOut(gpio::GPIO_PORT_t::PORTA, 7, gpio::GPIO_SPEED_t::LOW,
						gpio::GPIO_OUT_TYPE_t::PP, gpio::GPIO_PULL_TYPE_t::UP);
		gpio::ConfigAsOut(gpio::GPIO_PORT_t::PORTB, 0, gpio::GPIO_SPEED_t::LOW,
						gpio::GPIO_OUT_TYPE_t::PP, gpio::GPIO_PULL_TYPE_t::UP);

	}

	static void SetAll(uint8_t state) noexcept {
		gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 4, state);
		gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 6, state);
		gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 2, state);
		gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 5, state);
		gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 7, state);
		gpio::WritePin(gpio::GPIO_PORT_t::PORTB, 0, state);
	}

	static void Set(uint8_t nled, uint8_t state) noexcept {
		switch (nled)
		{
		case 1:
			gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 4, state);
			break;
		case 2:
			gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 6, state);
			break;
		case 3:
			gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 2, state);
			break;
		case 4:
			gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 5, state);
			break;
		case 5:
			gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 7, state);
			break;
		case 6:
			gpio::WritePin(gpio::GPIO_PORT_t::PORTB, 0, state);
			break;
		}
	}

	static void Set2(uint8_t nled, uint8_t state) noexcept {
			switch (nled)
			{
			case 1:
				gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 4, state);
				gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 6, state);
				break;
			case 2:
				gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 6, state);
				gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 2, state);
				break;
			case 3:
				gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 2, state);
				gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 5, state);
				break;
			case 4:
				gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 5, state);
				gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 7, state);
				break;
			case 5:
				gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 7, state);
				gpio::WritePin(gpio::GPIO_PORT_t::PORTB, 0, state);
				break;
			case 6:
				gpio::WritePin(gpio::GPIO_PORT_t::PORTB, 0, state);
				gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 4, state);
				break;
			}
		}

	static void Set3(uint8_t nled, uint8_t state) noexcept {
			switch (nled)
			{
			case 1:
				gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 4, state);
				gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 6, state);
				gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 2, state);
				break;
			case 2:
				gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 6, state);
				gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 2, state);
				gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 5, state);
				break;
			case 3:
				gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 2, state);
				gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 5, state);
				gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 7, state);
				break;
			case 4:
				gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 5, state);
				gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 7, state);
				break;
			case 5:
				gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 7, state);
				gpio::WritePin(gpio::GPIO_PORT_t::PORTB, 0, state);
				gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 4, state);
				break;
			case 6:
				gpio::WritePin(gpio::GPIO_PORT_t::PORTB, 0, state);
				gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 4, state);
				gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 6, state);
				break;
			}
		}

	static void SetPyton() noexcept {
			gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 4, 1);
			delay_ms(150);
			gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 4, 0);
			gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 6, 1);
			delay_ms(150);
			gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 6, 0);
			gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 2, 1);
			delay_ms(150);
			gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 4, 0);
			gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 5, 1);
			delay_ms(150);
			gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 5, 0);
			gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 7, 1);
			delay_ms(150);
			gpio::WritePin(gpio::GPIO_PORT_t::PORTA, 7, 0);
			gpio::WritePin(gpio::GPIO_PORT_t::PORTB, 0, 1);
			delay_ms(150);
			gpio::WritePin(gpio::GPIO_PORT_t::PORTB, 0, 0);
		}

};

typedef led_c led;

#endif /* INCLUDE_LED_HPP_ */
