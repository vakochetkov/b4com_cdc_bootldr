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

static constexpr uint8_t LED_NUM = 7;

class led_c {

static uint8_t ledStates[LED_NUM];

public:
	static void Init() noexcept {
		memset(led_c::ledStates, 0, sizeof(led_c::ledStates));

	}

	static void Set(uint8_t nled, uint8_t state) noexcept {

	}

	static uint8_t GetState(uint8_t nled) noexcept {

	}

	static void SetGroup(uint8_t ledGroup8) noexcept {

	}
};

typedef led_c led;

#endif /* INCLUDE_LED_HPP_ */
