/*
 * timeout.hpp
 *
 *  Created on: 13 дек. 2020 г.
 *      Author: kochetkov
 */

#ifndef INCLUDE_TIMEOUT_HPP_
#define INCLUDE_TIMEOUT_HPP_

#include "rcc.hpp"

enum class timeout_state_t : uint8_t {
	ST_IDLE = 0,
	ST_SET,
	ST_ALARM
};

/**
 * 1. Implement GetTick function returning number of current milliseconds
 * 2. Bind it to object of timeout_c class
 * NB: Note, all objects are independent but rely on the same GetTick function
 * NB: Non-IRQ version, milliseconds only
 */
template<auto FGetTick, typename TTick>
class timeout_c {
	TTick startTick;
	TTick interval;
	timeout_state_t state;

public:
	timeout_c() noexcept
	: startTick(0), interval(0), state(timeout_state_t::ST_IDLE)
	{	}

	void Set(TTick ms) noexcept {
		state = timeout_state_t::ST_SET;
		interval = ms;
		startTick = FGetTick();
	}

	void Update() noexcept {
		if ((FGetTick() - startTick) >= interval) {
			state = timeout_state_t::ST_ALARM;
		}
	}

	bool IsTimeOut() noexcept {
		if (state == timeout_state_t::ST_ALARM) {
			return true;
		} else {
			return false;
		}
	}

	inline void Clear() noexcept {
		startTick = 0;
		interval  = 0;
		state = timeout_state_t::ST_IDLE;
	}
};

typedef timeout_c<rcc::GetTick, uint32_t> Timeout;

#endif /* INCLUDE_TIMEOUT_HPP_ */
