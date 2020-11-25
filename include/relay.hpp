/*
 * relay.hpp
 *
 *  Created on: 24 нояб. 2020 г.
 *      Author: Alex
 */

#ifndef INCLUDE_RELAY_HPP_
#define INCLUDE_RELAY_HPP_


#include <cstring>
#include "common.hpp"
#include "gpio.hpp"

#ifndef RELAY_COUNT
	#error "Define RELAY_COUNT!"
#else
#if RELAY_COUNT > 12
	#error "Unsupported RELAY_COUNT!"
#else
	#warning "Check RELAY_COUNT is right"
#endif
#endif

class relay_c {

public:
	static void Init() noexcept {

	}

	static void Set(uint8_t nled, uint8_t state) noexcept {

	}

	static uint8_t GetState(uint8_t nled) noexcept {

	}

	static void SetGroup(uint8_t ledGroup8) noexcept {

	}
};

typedef relay_c relay;

#endif /* INCLUDE_RELAY_HPP_ */
